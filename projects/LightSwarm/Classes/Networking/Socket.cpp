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
	
	CCLOG("%s Disconnect socket id %d", (notifyDelegate ? "Forced" : "Manual"), _id);


	_isConnected = false;
	_isBound = false;
	if(_messageAcceptorData != NULL) {
	
		while(!_messageAcceptorData->children.empty()) {
			MessageReceiverData* childMessageReceiverData = _messageAcceptorData->children.front();
			close(*childMessageReceiverData->sockfd);
			delete childMessageReceiverData->sockfd;
			delete childMessageReceiverData;
			_messageAcceptorData->children.pop_front();
		}
	
		delete _messageAcceptorData;
		_messageAcceptorData = NULL;
	}
	
	if(_messageReceiverData != NULL) {
		delete _messageReceiverData;
		_messageReceiverData = NULL;
	}	
	
	if(notifyDelegate && _delegate != NULL) {
		_delegate->onDisconnect();
	}
}

void Socket::disconnectChild(MessageReceiverData* messageReceiverData) {

	close(*messageReceiverData->sockfd);
	*messageReceiverData->sockfd = 0;
	delete messageReceiverData->sockfd;
	
	delete messageReceiverData;				

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

bool Socket::hasChildren() {
	return _messageAcceptorData != NULL && !_messageAcceptorData->children.empty();
}


bool Socket::connectTo(const string &hostname, const int &port) {

	CCLOG("Connecting with socket id %d...", _id);
			
	if(!Socket::createSocket(&_sockfd)) {
		return false;
	}
	
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
	
	if (connect(_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR connecting");
		return false;
	}
	_isConnected = true;

	if(_messageReceiverData != NULL) {
		delete _messageReceiverData;
		_messageReceiverData = NULL;
	}

	//start our message receiver;
	_messageReceiverData = new MessageReceiverData();
	_messageReceiverData->socket = this;
	_messageReceiverData->sockfd = &_sockfd;
	_messageReceiverData->buffer = "";
	_messageReceiverData->isSocketReady = false;
	pthread_create(&_messageReceiverData->thread, NULL, &messageReceiver, _messageReceiverData);
	
	int waitCounter = 0;
	while(waitCounter++ < 500 && !_messageReceiverData->isSocketReady) {
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

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		int yes = 1;

		if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			CCLOGERROR("ERROR failed to enable socket address reuse");
			return false;
		}
		if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int)) == -1) {
			CCLOGERROR("ERROR failed to enable socket port reuse");
			return false;
		}
		if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
			CCLOGERROR("ERROR failed to disable socket timeout");
			return false;
		}
		if (setsockopt(*sockfd, SOL_SOCKET, SO_KEEPALIVE, &tv, sizeof(tv)) == -1) {
			CCLOGERROR("ERROR failed to enable keepalive");
			return false;
		}
	}
	
	return true;
}

bool Socket::listenOn(const int& port) {
	CCLOG("Binding with socket id %d...", _id);
					
	if(!Socket::createSocket(&_sockfd)) {
		return false;
	}
	
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	if (bind(_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR on binding");
		return false;
	}
	
	listen(_sockfd, 5);
	CCLOG("Bound to port %d", port);
	
	_isBound = true;
	
	if(_messageAcceptorData != NULL) {
	
		while(!_messageAcceptorData->children.empty()) {
			MessageReceiverData* childMessageReceiverData = _messageAcceptorData->children.front();
			close(*childMessageReceiverData->sockfd);
			delete childMessageReceiverData->sockfd;
			delete childMessageReceiverData;
			_messageAcceptorData->children.pop_front();
		}
	
		delete _messageAcceptorData;
		_messageAcceptorData = NULL;
	}
	
	//start our connection accepter
	_messageAcceptorData = new MessageReceiverData();
	_messageAcceptorData->socket = this;
	_messageAcceptorData->sockfd = &_sockfd;
	_messageAcceptorData->buffer = "";
	_messageAcceptorData->isSocketReady = true;
	pthread_create(&_messageAcceptorData->thread, NULL, &connectionAccepter, _messageAcceptorData);
	
	return true;
}



void Socket::sendMessage(const Json::Value& message) {
	Json::FastWriter writer;
	string messageStr = writer.write(message);
	sendMessage(messageStr);
}

void Socket::sendMessage(const string& message) {

	if(_sockfd == 0) {
		return;
	}
	
	//TODO: handle the case when n < the sent data size (buffer!)
	int n = write(_sockfd, message.c_str(), message.length());
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
	
	while(messageReceiverData->socket != NULL && *messageReceiverData->sockfd > 0) {
		int newsockfd = accept(*messageReceiverData->sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			CCLOGERROR("ERROR on accept");
			return false;
		}
		CCLOG("Accepted new socket connection on socket %d", newsockfd);
		
		//start our message receiver;
		MessageReceiverData* messageReceiverDataCopy = new MessageReceiverData();
		messageReceiverDataCopy->socket = messageReceiverData->socket;
		messageReceiverDataCopy->sockfd = new int(newsockfd);
		messageReceiverDataCopy->buffer = "";
		messageReceiverDataCopy->isSocketReady = false;
		pthread_create(&messageReceiverDataCopy->thread, NULL, &messageReceiver, messageReceiverDataCopy);

		int waitCounter = 0;
		while(waitCounter++ < 500 && !messageReceiverDataCopy->isSocketReady) {
			usleep(1000);
		}
		
		if(waitCounter >= 500) {
			CCLOGERROR("Accepted successfully but failed to setup reading from socket from socket id %d in time", messageReceiverData->socket->getId());
			close(*messageReceiverData->sockfd);
			delete messageReceiverDataCopy->sockfd;
			delete messageReceiverDataCopy;
			return false;
		}else {
			messageReceiverData->children.push_back(messageReceiverDataCopy);
		}
		
	}
	pthread_exit(0);
}

void* messageReceiver(void* threadData) {

	MessageReceiverData* messageReceiverData = (MessageReceiverData*)threadData;

	Json::Reader reader;
	Json::Value root;
			
	int numFailedReads = 0;
	int bytesRecv;
	char buffer[1024];
	
	
	CCLOG("Waiting for data from socket %d...", *messageReceiverData->sockfd);
	while(messageReceiverData->socket != NULL && *messageReceiverData->sockfd > 0) {

		bzero(buffer, 1024);
		messageReceiverData->isSocketReady = true;
		
		bytesRecv = read(*messageReceiverData->sockfd, buffer, 1023);
		
		if (bytesRecv < 0) {
			CCLOGERROR("ERROR reading from socket");
			messageReceiverData->isSocketReady = false;
			if(messageReceiverData->socket != NULL) {
				if(messageReceiverData->socket->getSockFd() == *messageReceiverData->sockfd) {
					messageReceiverData->socket->disconnect(true);
				}else {
					messageReceiverData->socket->disconnectChild(messageReceiverData);
				}
			}
			break;
			
		}else if(bytesRecv == 0) {
			if(numFailedReads++ >= 100) {
				CCLOGERROR("ERROR reading from socket (numFailedReads=%d)", numFailedReads);
				messageReceiverData->isSocketReady = false;
				if(messageReceiverData->socket != NULL) {
					if(messageReceiverData->socket->getSockFd() == *messageReceiverData->sockfd) {
						messageReceiverData->socket->disconnect(true);
					}else {
						messageReceiverData->socket->disconnectChild(messageReceiverData);
					}
				}
				break;
			}
			
		}else {
			if(messageReceiverData->socket == NULL) {
				messageReceiverData->isSocketReady = false;
				break;
			}
			//CCLOG("read: %s", buffer);
			messageReceiverData->buffer+= buffer;
			//CCLOG("messageReceiverData->buffer: %s", messageReceiverData->buffer.c_str());
						
			int endMessagePos = messageReceiverData->buffer.find(MESSAGE_TERMINATOR);
			while(endMessagePos != string::npos) {
						
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
	}
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



