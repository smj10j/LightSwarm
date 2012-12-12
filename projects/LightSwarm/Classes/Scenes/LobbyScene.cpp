//
//  LobbyScene.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#include "LobbyScene.h"
#include "GameScene.h"


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
	
	CCLog("Lobby loaded at %f", Utilities::getMillis());
	
	this->setTouchEnabled(true);
	
	_clientSocket = new ClientSocket(this);
	if(!_clientSocket->connectTo(LOBBY_SERVER, LOBBY_PORT)) {
		CCLOGERROR("FAILED to Connect to Lobby Server");
	}
		
	return true;
}

void LobbyScene::onMessage(const Json::Value& message) {
	CCLOG("LobbyScene received message: %s", message.toStyledString().c_str());
}

void LobbyScene::onConnect() {
	CCLOG("Connected to Lobby Server");
	
	//TODO: identify
	string message = "{}";
	_clientSocket->sendMessage(message);
}

void LobbyScene::onDisconnect() {
	CCLOG("Disconnected from Lobby Server");		

	//TODO: notify user of lost connection
	
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































LobbyScene::~LobbyScene() {
	if(_clientSocket != NULL) {
		delete _clientSocket;
		_clientSocket = NULL;
	}
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

