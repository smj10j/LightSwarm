//
//  Socket.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/12/12.
//
//

#ifndef __LightSwarm__Socket__
#define __LightSwarm__Socket__

#include "Common.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "json.h"
#include <list>

#include <ifaddrs.h>
#include <arpa/inet.h>


USING_NS_CC;



struct MessageReceiverData;
void* messageReceiver(void* threadData);
void* connectionAccepter(void* threadData);

class SocketDelegate {
public:
	virtual void onMessage(const Json::Value& message) {};
	virtual void onConnect() {};
	virtual void onDisconnect() {};
	virtual void onDisconnectChild() {};
};

static int SOCKET_ID = 1;

class Socket {

public:

	Socket():
		_id(SOCKET_ID++),
		_sockfd(0),
		_isConnected(false),
		_delegate(NULL),
		_messageReceiverData(NULL) {
		
	}

	Socket(SocketDelegate* delegate):
		_id(SOCKET_ID++),
		_sockfd(0),
		_isConnected(false),
		_delegate(delegate),
		_messageReceiverData(NULL) {
		
	}
	
	int getId() { return _id; };
	int getSockFd() { return _sockfd; };
	
	bool isConnected() { return _isConnected; };

	bool listenOn(const int& port);
	bool connectTo(const string& hostname, const int& port);
	void disconnect(bool notifyDelegate);
	void disconnectChild(MessageReceiverData* messageReceiverData);
	void sendMessage(const Json::Value& message);
	void sendMessage(const string& message);
	
	void onMessage(const Json::Value message);
	
	static string getLocalIPAddress();
	static bool createSocket(int* sockfd);
		
	virtual ~Socket();

private:

	int _sockfd;
	bool _isConnected;
	MessageReceiverData* _messageReceiverData;
	
	SocketDelegate* _delegate;
	
	int _id;
};


struct MessageReceiverData {
	pthread_t thread;
	bool isSocketReady;
	string buffer;
	int* sockfd;
	Socket* socket;
	list<MessageReceiverData*> children;
};



#define MESSAGE_TERMINATOR "~|~"

#endif /* defined(__LightSwarm__Socket__) */
