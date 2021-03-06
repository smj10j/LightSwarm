//
//  Socket.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/12/12.
//
//

#include "Socket.h"

void Socket::disconnect(bool notifyDelegate) {

	if(_sockfd <= 0) {
		return;
	}

	close(_sockfd);
	_sockfd = 0;
	CCLOG("Set sockfd=0 in disconnect");
	
	CCLOG("%s Disconnect socket id %d", (notifyDelegate ? "Forced" : "Manual"), _id);


	_isConnected = false;
	_isBound = false;
	if(_messageAcceptorData != NULL) {

		if(*_messageAcceptorData->sockfd > 0) {
			//pthread_kill(_messageAcceptorData->thread, 0);
			close(*_messageAcceptorData->sockfd);
			*_messageAcceptorData->sockfd = 0;
		
			while(!_messageAcceptorData->children.empty()) {
				MessageReceiverData* childMessageReceiverData = _messageAcceptorData->children.front();
				//pthread_kill(childMessageReceiverData->thread, 0);
				close(*childMessageReceiverData->sockfd);
				*childMessageReceiverData->sockfd = 0;
				_messageAcceptorData->children.pop_front();
			}
		}
	
		_messageAcceptorData = NULL;
	}
	
	if(_messageReceiverData != NULL) {
		if(*_messageReceiverData->sockfd > 0) {
			//pthread_kill(_messageReceiverData->thread, 0);
			close(*_messageReceiverData->sockfd);
			*_messageReceiverData->sockfd = 0;
		}
		_messageReceiverData = NULL;
	}	
	
	if(notifyDelegate && _delegate != NULL) {
		_delegate->onDisconnect();
	}
}

void Socket::disconnectChild(MessageReceiverData* messageReceiverData) {

	if(messageReceiverData != NULL) {
		if(*messageReceiverData->sockfd > 0) {
			close(*messageReceiverData->sockfd);
			*messageReceiverData->sockfd = 0;
			CCLOG("Set sockfd=0 in disconnectChild");
		}
		messageReceiverData = NULL;
	}

	if(_delegate != NULL) {
		_delegate->onDisconnectChild();
	}	
}

int Socket::getLocalPort() {
	struct sockaddr_in sin;
	socklen_t addrlen = sizeof(sin);
	if(getsockname(_sockfd, (struct sockaddr *)&sin, &addrlen) == 0 &&
	   sin.sin_family == AF_INET &&
	   addrlen == sizeof(sin)) {
		return ntohs(sin.sin_port);
	} else {
		CCLOGERROR("Failed to get local port");
		return 0;
	}
}

bool Socket::connectTo(const string &hostname, const int &port, int timeout) {

	CCLOG("Connecting with socket id %d...", _id);

	if(_messageReceiverData != NULL) {
		if(*_messageReceiverData->sockfd > 0) {
			//pthread_kill(_messageReceiverData->thread, 0);
			close(*_messageReceiverData->sockfd);
			*_messageReceiverData->sockfd = 0;
			CCLOG("Set sockfd=0 in connectTo");
		}
		_messageReceiverData = NULL;
	}
	
	if(!Socket::createSocket(&_sockfd)) {
		return false;
	}
	
	//set socket to nonblocking
	if(_sockfdFlags == -1) {
		_sockfdFlags = fcntl(_sockfd, F_GETFL, 0);
	}
    fcntl(_sockfd, F_SETFL, _sockfdFlags | O_NONBLOCK);
	
	struct hostent *server;
	server = gethostbyname(hostname.c_str());
	if (server == NULL) {
		CCLOGERROR("ERROR, no such host \"%s\"", hostname.c_str());
		return false;
	}
	
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	
	//perform a select to do a timeout
	connect(_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));

	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(_sockfd, &rfd);
	struct timeval tv;
	tv.tv_sec = timeout;	//X second timeout
	tv.tv_usec = 0;
	int retVal = select(_sockfd+1, NULL, &rfd, NULL, &tv);
    CCLOG("select retVal: %d", retVal);

	bool success = false;
	if (retVal == 1) {
		int so_error;
        socklen_t slen = sizeof so_error;
        getsockopt(_sockfd, SOL_SOCKET, SO_ERROR, &so_error, &slen);
        if (so_error == 0) {
            CCLOG("Successfully connected");
            success = true;
        } else {
            CCLOGERROR("Failed to connect - error: %d", so_error);
            success = false;
        }
    } else {     
        CCLOGERROR("No socket??"); 
    }
	
	
	if(!success) {
		return false;
	}
	
	//re-enable blocking
    //fcntl(_sockfd, F_SETFL, _sockfdFlags);
		
	_isConnected = true;

	//start our message receiver;
	_messageReceiverData = new MessageReceiverData();
	_messageReceiverData->socket = this;
	_messageReceiverData->sockfd = &_sockfd;
	_messageReceiverData->sockFdFlags = &_sockfdFlags;
	_messageReceiverData->buffer = "";
	_messageReceiverData->isSocketReady = false;
	pthread_create(&_messageReceiverData->thread, NULL, &messageReceiver, _messageReceiverData);
	
	int waitCounter = 0;
	while(waitCounter++ < 500 && _messageReceiverData != NULL && !_messageReceiverData->isSocketReady) {
		usleep(1000);
	}
	
	if(waitCounter >= 500) {
		CCLOGERROR("Connected successfully but failed to setup reading from socket in time");
		disconnect(false);
		return false;
	}
	
	if(_delegate != NULL) {
		_delegate->onConnect();
	}
	
	return true;
}

bool Socket::createSocket(int* sockfd) {
	
	if(*sockfd <= 0) {
		*sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (*sockfd < 0) {
			CCLOGERROR("ERROR opening socket");
			return false;
		}

		struct timeval tv0;
		tv0.tv_sec = 0;
		tv0.tv_usec = 0;
		struct timeval tv2;
		tv2.tv_sec = 2;
		tv2.tv_usec = 0;
		int yes = 1;

		if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
			CCLOGERROR("ERROR failed to enable socket address reuse");
			return false;
		}
		if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
			CCLOGERROR("ERROR failed to enable socket port reuse");
			return false;
		}
		if (setsockopt(*sockfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(tv0)) == -1) {
			CCLOGERROR("ERROR failed to enable keepalive");
			return false;
		}
		if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv0, sizeof(tv0)) == -1) {
			CCLOGERROR("ERROR failed to disable read socket timeout");
			return false;
		}
		if (setsockopt(*sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv2, sizeof(tv2)) == -1) {
			CCLOGERROR("ERROR failed to enable send socket timeout");
			return false;
		}
	}
	
	return true;
}

bool Socket::listenOn(const int& port) {
	CCLOG("Binding with socket id %d...", _id);

	disconnect(false);

	if(!Socket::createSocket(&_sockfd)) {
		return false;
	}
	
	if(_sockfdFlags == -1) {
		_sockfdFlags = fcntl(_sockfd, F_GETFL, 0);
	}
    fcntl(_sockfd, F_SETFL, _sockfdFlags);
	
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	if (::bind(_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR on binding");
		return false;
	}
	
	listen(_sockfd, 5);
	CCLOG("Bound to port %d with socket %d", port, _sockfd);
	
	fcntl(_sockfd, F_SETFL, _sockfdFlags | O_NONBLOCK);
	
	_isBound = true;
	
	//start our connection accepter
	_messageAcceptorData = new MessageReceiverData();
	_messageAcceptorData->socket = this;
	_messageAcceptorData->sockfd = &_sockfd;
	_messageAcceptorData->sockFdFlags = &_sockfdFlags;
	_messageAcceptorData->buffer = "";
	_messageAcceptorData->isSocketReady = true;
	pthread_create(&_messageAcceptorData->thread, NULL, &connectionAccepter, _messageAcceptorData);
	
	return true;
}


void Socket::sendMessage(const Json::Value& message) {
	sendMessage(message, true);
}

void Socket::sendMessage(const Json::Value& message, bool addTerminator) {
	Json::FastWriter writer;
	string messageStr = writer.write(message);
	sendMessage(messageStr + (addTerminator ? MESSAGE_TERMINATOR : ""));
}

void Socket::sendMessage(const string& message) {

	if(_sockfd == 0) {
		return;
	}
		
	//TODO: handle the case when n < the sent data size (buffer!)
	int n = send(_sockfd, message.c_str(), message.length(), 0);
	if (n < 0) {
		CCLOGERROR("ERROR writing to socket");
		disconnect(true);
	}
}

void Socket::onMessage(const Json::Value message) {
	if(_delegate != NULL) {
		_delegate->onMessage(message);
	}
}

void* connectionAccepter(void* threadData) {
	MessageReceiverData* messageReceiverData = (MessageReceiverData*)threadData;

	socklen_t clilen;
	struct sockaddr_in cli_addr;
	clilen = sizeof(cli_addr);
		
	while(messageReceiverData->socket != NULL && *messageReceiverData->sockfd > 0 && messageReceiverData->socket->isBound()) {
		int sockfd = *messageReceiverData->sockfd;

		fcntl(sockfd, F_SETFL, *messageReceiverData->sockFdFlags | O_NONBLOCK);
		
		fd_set rfd;
		FD_ZERO(&rfd);
		FD_SET(sockfd, &rfd);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 20000;	//20ms read timeout
		int retVal = select(sockfd+1, &rfd, NULL, NULL, &tv);
		//CCLOG("read accept select retVal: %d", retVal);

		if (retVal > 0) {		
		
			if(FD_ISSET(sockfd, &rfd)) {
				int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
				if(*messageReceiverData->sockfd <= 0) {
					break;
				}
				
				if (newsockfd < 0) {
					CCLOGERROR("ERROR on accept - newsockfd=%d", newsockfd);
					return false;
				}
				CCLOG("Accepted new socket connection on socket %d", newsockfd);

				fcntl(newsockfd, F_SETFL, *messageReceiverData->sockFdFlags | O_NONBLOCK);
							
				//start our message receiver;
				MessageReceiverData* messageReceiverDataCopy = new MessageReceiverData();
				messageReceiverDataCopy->socket = messageReceiverData->socket;
				messageReceiverDataCopy->sockfd = new int(newsockfd);
				messageReceiverDataCopy->sockFdFlags = messageReceiverData->sockFdFlags;
				messageReceiverDataCopy->buffer = "";
				messageReceiverDataCopy->isSocketReady = false;
				pthread_create(&messageReceiverDataCopy->thread, NULL, &messageReceiver, messageReceiverDataCopy);

				int waitCounter = 0;
				while(waitCounter++ < 500 && !messageReceiverDataCopy->isSocketReady) {
					usleep(1000);
				}
				
				if(waitCounter >= 500) {
					CCLOGERROR("Accepted successfully but failed to setup reading from socket from socket id %d in time", messageReceiverData->socket->getId());
					close(*messageReceiverDataCopy->sockfd);
					*messageReceiverDataCopy->sockfd = 0;
					return false;
				}else {
					messageReceiverData->socket->setConnected(true);
					messageReceiverData->children.push_back(messageReceiverDataCopy);
				}
			}else {
				CCLOGERROR("Select socket doesn't match accepted rfd?");
			}
			
		}else if (retVal == 0) {
			//CCLOGERROR("Timeout on accept");
			
		}else {
			CCLOGERROR("ERROR on select for accept");
			
		}
	}

	CCLOG("Left connection accepting thread with sockfd=%d", *messageReceiverData->sockfd);

	close(*messageReceiverData->sockfd);
	*messageReceiverData->sockfd = 0;
	delete messageReceiverData;
	
	pthread_exit(0);
}

void* messageReceiver(void* threadData) {

	MessageReceiverData* messageReceiverData = (MessageReceiverData*)threadData;

	Json::Reader reader;
	Json::Value root;
			
	int numFailedReads = 0;
	int bytesRecv;
	char buffer[1024];
	bool isParent = true;
	
	CCLOG("Waiting for data from socket %d... %f", *messageReceiverData->sockfd, Utilities::getMillis());
	while(*messageReceiverData->sockfd > 0 && messageReceiverData->socket != NULL &&
		(!messageReceiverData->isSocketReady || messageReceiverData->socket->isConnected())) {

		bzero(buffer, 1024);
		messageReceiverData->isSocketReady = true;
		
		int sockfd = *messageReceiverData->sockfd;
		isParent = messageReceiverData->socket->getSockFd() == sockfd;

		//CCLOG("Waiting for data from from socket %d - %f", sockfd, Utilities::getMillis());
		
		fd_set rfd;
		FD_ZERO(&rfd);
		FD_SET(sockfd, &rfd);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 20000;	//20ms read timeout
		int retVal = select(sockfd+1, &rfd, NULL, NULL, &tv);
		//CCLOG("read select retVal: %d", retVal);

		if (retVal > 0) {
		

			bytesRecv = recv(sockfd, buffer, 1023, 0);
			//CCLOG("read BYTES: %d", bytesRecv);
			if(*messageReceiverData->sockfd <= 0 || messageReceiverData->socket == NULL) {
				messageReceiverData->isSocketReady = false;
				break;
			}
			
			if (bytesRecv < 0) {
				CCLOGERROR("ERROR reading from socket %d - %f", sockfd, Utilities::getMillis());
				messageReceiverData->isSocketReady = false;
				if(isParent) {
					messageReceiverData->socket->disconnect(true);
				}else {
					messageReceiverData->socket->disconnectChild(messageReceiverData);
				}
				break;
				
			}else if(bytesRecv == 0) {
				if(numFailedReads++ >= 100) {
					CCLOGERROR("ERROR reading from socket %d (numFailedReads=%d)", sockfd, numFailedReads);
					messageReceiverData->isSocketReady = false;
					if(isParent) {
						messageReceiverData->socket->disconnect(true);
					}else {
						messageReceiverData->socket->disconnectChild(messageReceiverData);
					}
					break;
				}
				
			}else {

				CCLOG("read: %s, retVal=%d, bytesRecv=%d", buffer, retVal, bytesRecv);
				messageReceiverData->buffer+= buffer;
				//CCLOG("messageReceiverData->buffer: %s", messageReceiverData->buffer.c_str());
							
				int endMessagePos = messageReceiverData->buffer.find(MESSAGE_TERMINATOR);
				while(endMessagePos != string::npos && messageReceiverData->buffer != "") {
							
					//pull out the parsed message
					string singleMessage = messageReceiverData->buffer.substr(0, endMessagePos);
					messageReceiverData->buffer = messageReceiverData->buffer.substr(endMessagePos+strlen(MESSAGE_TERMINATOR));
					endMessagePos = messageReceiverData->buffer.find(MESSAGE_TERMINATOR);
				
					//parse JSON
					if(reader.parse(singleMessage, root)) {
						//send off the message to the onMessage delegate
						messageReceiverData->socket->onMessage(root);
					}else {
						CCLOGERROR("Failed to parse message: %s", singleMessage.c_str());
					}
				}
				
				numFailedReads = 0;
			}
			
		} else if(retVal == 0) {
			//CCLOGERROR("Read timeout");
			
		} else {
			CCLOGERROR("Error while polling read socket");

		}
		
	}
	
	/*
	close(*messageReceiverData->sockfd);
	CCLOG("Set sockfd=0 in messageReceiver");
	*messageReceiverData->sockfd = 0;
	*/
	if(!isParent) {
		delete messageReceiverData->sockfd;
	}
	delete messageReceiverData;
		
	pthread_exit(0);
}


string Socket::getLocalIPAddress() {

	string address;

	struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
		
		string targetInterface = "en0";
		
		while(targetInterface != "" && address == "") {
			while(temp_addr != NULL) {
				if(temp_addr->ifa_addr->sa_family == AF_INET) {
					//CCLOG("temp_addr->ifa_name = %s", string(temp_addr->ifa_name).c_str());
					//CCLOG("address = %s", string(inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)).c_str());
					// Check if interface is en0 which is the wifi connection on the iPhone
					if(string(temp_addr->ifa_name) == targetInterface) {
						// Get string from C String
						address = inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr);
					}

				}

				temp_addr = temp_addr->ifa_next;
			}
			if(targetInterface == "en0") {
				targetInterface = "en1";
			}else if(targetInterface == "en1") {
				targetInterface = "pdp_ip0";	//3G
			}else {
				targetInterface = "";
			}
			temp_addr = interfaces;
		}
    }
    // Free memory
    freeifaddrs(interfaces);
	
	return address;
}


Socket::~Socket() {
	disconnect(false);
}



