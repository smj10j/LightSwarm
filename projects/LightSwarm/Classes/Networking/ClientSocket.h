//
//  ClientSocket.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/12/12.
//
//

#ifndef __LightSwarm__ClientSocket__
#define __LightSwarm__ClientSocket__

#include "Common.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "json.h"

USING_NS_CC;



struct MessageReceiverData;
void* messageReceiver(void* threadData);

class ClientSocketDelegate {
public:
	virtual void onMessage(const Json::Value& message) {};
	virtual void onConnect() {};
	virtual void onDisconnect() {};
};

class ClientSocket {

public:

	ClientSocket():
		_sockfd(0),
		_isConnected(false),
		_delegate(NULL),
		_messageReceiverData(NULL) {
		
	}

	ClientSocket(ClientSocketDelegate* delegate):
		_sockfd(0),
		_isConnected(false),
		_delegate(delegate),
		_messageReceiverData(NULL) {
		
	}
	
	bool isConnected() { return _isConnected; };

	bool connectTo(const string& hostname, const int& port);
	void disconnect(bool notifyDelegate);
	void sendMessage(const Json::Value& message);
	void sendMessage(const string& message);
	
	void onMessage(const Json::Value message);
	
	virtual ~ClientSocket();

private:

	int _sockfd;
	bool _isConnected;
	MessageReceiverData* _messageReceiverData;
	
	ClientSocketDelegate* _delegate;
	
	

};


struct MessageReceiverData {
	bool isSocketReady;
	string buffer;
	int* sockfd;
	ClientSocket* clientSocket;
};

#define MESSAGE_TERMINATOR "~|~"

#endif /* defined(__LightSwarm__ClientSocket__) */
