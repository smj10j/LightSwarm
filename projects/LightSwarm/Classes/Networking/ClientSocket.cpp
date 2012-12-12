//
//  ClientSocket.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/12/12.
//
//

#include "ClientSocket.h"


//TODO: add ability to detect graceful and forced disconnected (so we can alert the delegate when we receivied an unexpected disconnect)
void ClientSocket::disconnect() {

	CCLOG("Disconnecting");

	if(_sockfd != 0) {
		close(_sockfd);
		_sockfd = 0;
	}
	_isConnected = false;
	if(_messageReceiverData != NULL) {
		delete _messageReceiverData;
		_messageReceiverData = NULL;
	}
}

bool ClientSocket::connectTo(const string &hostname, const int &port) {

	CCLOG("Connecting...");
	
	disconnect();
	
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		CCLOGERROR("ERROR opening socket");
		return false;
	}
	
	server = gethostbyname(hostname.c_str());
	if (server == NULL) {
		CCLOGERROR("ERROR, no such host \"%s\"", hostname.c_str());
		return false;
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	
	if (connect(_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR connecting");
		return false;
	}
	_isConnected = true;

	//start our message receiver;
	pthread_t messageReceiverThread;
	_messageReceiverData = new MessageReceiverData();
	_messageReceiverData->clientSocket = this;
	_messageReceiverData->sockfd = &_sockfd;
	_messageReceiverData->buffer = "";
	_messageReceiverData->isSocketReady = false;
	pthread_create(&messageReceiverThread, NULL, &messageReceiver, _messageReceiverData);
	
	int waitCounter = 0;
	while(waitCounter++ < 100 && !_messageReceiverData->isSocketReady) {
		usleep(25);
	}
	
	if(waitCounter >= 100) {
		CCLOGERROR("Connected successfully but failed to setup reading from socket in time");
		disconnect();
		return false;
	}
	
	return true;
}



void ClientSocket::sendMessage(const string& message) {

	if(_sockfd == 0) {
		return;
	}

	//TODO: handle the case when n < the sent data size (buffer!)
	int n = write(_sockfd, message.c_str(), message.length());
	if (n < 0) {
		CCLOGERROR("ERROR writing to socket");
	}
}

void ClientSocket::onMessage(const Json::Value message) {
	if(_delegate != NULL) {
		_delegate->onMessage(message);
	}
}

void* messageReceiver(void* threadData) {

	MessageReceiverData* messageReceiverData = (MessageReceiverData*)threadData;

	Json::Reader reader;
	Json::Value root;
			
	int numFailedReads = 0;
	int bytesRecv;
	char buffer[1024];
	
	
	CCLOG("Waiting for data from socket %d...", messageReceiverData->sockfd);
	while(messageReceiverData->clientSocket != NULL && messageReceiverData->sockfd > 0) {

		bzero(buffer, 1024);
		messageReceiverData->isSocketReady = true;
		
		bytesRecv = read(*messageReceiverData->sockfd, buffer, 1023);
		
		if (bytesRecv < 0) {
			CCLOGERROR("ERROR reading from socket");
			messageReceiverData->isSocketReady = false;
			if(messageReceiverData->clientSocket != NULL) {
				messageReceiverData->clientSocket->disconnect();
			}
			break;
			
		}else if(bytesRecv == 0) {
			if(numFailedReads++ >= 100) {
				CCLOGERROR("ERROR reading from socket (numFailedReads=%d)", numFailedReads);
				messageReceiverData->isSocketReady = false;
				if(messageReceiverData->clientSocket != NULL) {
					messageReceiverData->clientSocket->disconnect();
				}
				break;
			}
			
		}else {
			if(messageReceiverData->clientSocket == NULL) {
				messageReceiverData->isSocketReady = false;
				break;
			}
			messageReceiverData->buffer+= buffer;
						
			int endMessagePos = messageReceiverData->buffer.find(MESSAGE_TERMINATOR);
			if(endMessagePos != string::npos) {
			
				//pull out the parsed message
				string singleMessage = messageReceiverData->buffer.substr(0, endMessagePos);
				messageReceiverData->buffer = messageReceiverData->buffer.substr(endMessagePos+strlen(MESSAGE_TERMINATOR));
			
				//parse JSON
				if(reader.parse(singleMessage, root)) {
					//send off the message to the onMessage delegate
					messageReceiverData->clientSocket->onMessage(root);
				}else {
					CCLOGERROR("Failed to parse message: %s", singleMessage.c_str());
				}
			}
			
			numFailedReads = 0;
		}
	}
	pthread_exit(0);
}





ClientSocket::~ClientSocket() {
	disconnect();
}



