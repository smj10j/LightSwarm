//
//  LobbyScene.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#include "LobbyScene.h"
#include "GameScene.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void* messageReceiver(void* threadData);



CCScene* LobbyScene::scene() {
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    LobbyScene *layer = LobbyScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LobbyScene::init() {
    if ( !CCLayer::init() ) {
        return false;
    }

	//default blend function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();


	//create a batch node
	_batchNode = CCSpriteBatchNode::create("Sprites.pvr.ccz");
	this->addChild(_batchNode);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Sprites.plist");
	
	_sockfd = 0;
	connectToLobbyServer();

	CCLog("Lobby loaded at %f", Utilities::getMillis());
	
	this->setTouchEnabled(true);		
		
	return true;
}

void LobbyScene::onConnect() {
	CCLOG("LobbyScene:onConnect()");
	
	string message = "{}";
	sendMessage(message);
}

void LobbyScene::onSuccess(RESPONSE_TYPE type, string message) {
	CCLOG("Got success response with message %s", message.c_str());
	
	//TODO:test hmac
	CCLOG("testing hmac...");
	//Utilities::hmac_sha1("bubba", "gump");
}



void LobbyScene::onError(RESPONSE_TYPE type, string error) {
	CCLOG("Got error response with message %s", error.c_str());
}


void LobbyScene::onEnter() {
	CCLOG("On Enter LobbyScene");
	this->CCLayer::onEnter();
	
	//loadGameScene();
	
	
	
	
}

void LobbyScene::loadGameScene() {
	CCDirector::sharedDirector()->replaceScene(CCTransitionFadeBL::create(
		0.5,
		GameScene::scene(
			
		)
	));
}


void LobbyScene::onExit() {
	CCLOG("On Exit LobbyScene");
	this->CCLayer::onExit();
}



void LobbyScene::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

}

void LobbyScene::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

}

void LobbyScene::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {


}















void LobbyScene::disconnectFromLobbyServer() {

	if(_sockfd != 0) {
		close(_sockfd);
		_sockfd = 0;
	}

}

bool LobbyScene::connectToLobbyServer() {

	CCLOG("Connecting...");
	
	disconnectFromLobbyServer();
	
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		CCLOGERROR("ERROR opening socket");
		return false;
	}
	
	server = gethostbyname(LOBBY_SERVER);
	if (server == NULL) {
		CCLOGERROR("ERROR, no such host \"%s\"", LOBBY_SERVER);
		return false;
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(LOBBY_PORT);
	if (connect(_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR connecting");
		return false;
	}

	//start our message receiver;
	pthread_t messageReceiverThread;
	pthread_create(&messageReceiverThread, NULL, &messageReceiver, &_sockfd);
	
	scheduleOnce(schedule_selector(LobbyScene::onConnect), .05);

	return false;
}



void LobbyScene::sendMessage(const string& message) {

	if(_sockfd == 0) {
		return;
	}

	//TODO: handle the case when n < the sent data size (buffer!)
	int n = write(_sockfd, message.c_str(), message.length());
	if (n < 0) {
		CCLOGERROR("ERROR writing to socket");
	}
}

void* messageReceiver(void* threadData) {

	int* sockfd = (int*)(threadData);

	if(*sockfd == 0) {
		return NULL;
	}

	int numFailedReads = 0;
	int n;
	char buffer[256];
	bzero(buffer, 256);
	
	CCLOG("Waiting for data from socket %d...", *sockfd);
	while(sockfd > 0) {
		n = read(*sockfd, buffer, 255);
		if (n < 0) {
			CCLOGERROR("ERROR reading from socket");
			close(*sockfd);
			*sockfd = 0;
			break;
		}else if(n == 0) {
			if(numFailedReads++ > 100) {
				CCLOGERROR("ERROR reading from socket (numFailedReads=%d)", numFailedReads);
				close(*sockfd);
				*sockfd = 0;
				break;
			}
		}else {
			CCLOG("Received: %s",buffer);
			numFailedReads = 0;
		}
	}
	pthread_exit(0);
}

























LobbyScene::~LobbyScene() {
	disconnectFromLobbyServer();
}














/*
// Socket server example

	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	const int port = 3001;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		CCLOGERROR("ERROR opening socket");
	}
	CCLOG("Opened socket");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR on binding");
	}
	
	listen(sockfd, 5);
	CCLOG("Bound to port %d", port);
	
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) {
		CCLOGERROR("ERROR on accept");
	}
	CCLOG("Accepted new socket connection");
	
	bzero(buffer, 256);
	n = read(newsockfd, buffer, 255);
	if (n < 0) {
		CCLOGERROR("ERROR reading from socket");
	}
	CCLOG("Received: %s\n", buffer);
	
	n = write(newsockfd, "I got your message", 18);
	if (n < 0) {
		CCLOGERROR("ERROR writing to socket");
	}
	
	close(newsockfd);
	close(sockfd);	
*/



/*
//Socket client example

	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	const int port = 3001;
	char buffer[256];
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		CCLOGERROR("ERROR opening socket");
	}
	
	server = gethostbyname("localhost");
	if (server == NULL) {
		CCLOGERROR("ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(port);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR connecting");
	}
		
	bzero(buffer,256);
	strcpy(buffer, "test message");
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) {
		CCLOGERROR("ERROR writing to socket");
	}
	
	//bzero(buffer,256);
	//n = read(sockfd,buffer,255);
	//if (n < 0) {
	//	CCLOGERROR("ERROR reading from socket");
	//}
	//CCLOG("%s\n",buffer);
	close(sockfd);
*/

