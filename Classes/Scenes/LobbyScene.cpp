//
//  LobbyScene.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#include "LobbyScene.h"


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
		
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();	

	//create a batch node
	_batchNode = CCSpriteBatchNode::create("Sprites.png");
	this->addChild(_batchNode);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Sprites.plist");
	
	CCLog("Lobby loaded at %f", Utilities::getMillis());
	
	this->setTouchEnabled(true);
	
	//TODO: get this from the UI
#if TARGET_IPHONE_SIMULATOR
	_userId = "steve1";
	_friendUserId = "steve2";
#else
	_userId = "steve2";
	_friendUserId = "steve1";
#endif

	_socket = new Socket(this);
		
    _gameScene = GameScene::create();
	_gameScene->_player = new Player();
	_gameScene->_opponent = (Opponent*)new NetworkedOpponent(_socket);
	
		
	return true;
}

void LobbyScene::onMessage(const Json::Value& message) {
	CCLOG("LobbyScene received message: %s", message.toStyledString().c_str());
	
		
	Json::Value status = message["status"];
	if(status != NULL) {
	
		if(status.asString() == "ok") {

			Json::Value gameInit = message["gameInit"];
			if(gameInit != NULL && !gameInit.empty()) {

				Json::Value player1 = gameInit["player1"];
				Json::Value player2 = gameInit["player2"];
				
				//TODO: hole punch and connect!!
				//unless player1 cannot be the server, player1 is always the server
				
				//TODO:
					//gameScene init with:
					//	(player:Player, opponent:Opponent, isServer:bool, serverAddess:sockaddr_in)
					

				_socket->setDelegate((SocketDelegate*)_gameScene);
				_socket->disconnect(false);

				if(player1["userId"].asString() == _userId) {
					//we are player1 - connect to player2


					//try to connect to both public and private ips in a while loop
					for(int i = 0; i < 50; i++) {
					
						CCLOG("Waiting for connection from %s on port %d", player2["publicIP"].asCString(), _localPort);

						if(!_socket->listenOn(_localPort)) {
							CCLOGERROR("FAILED to Listen on Game Server port %d", _localPort);
						}					
						usleep(arc4random()%750000);
						if(_socket->isConnected()) {
							loadGameScene();
							break;
						}
						CCLOG("-------Manual disconnect");
						_socket->disconnect(false);
											
						CCLOG("Connecting to %s:%d", player2["publicIP"].asCString(), player2["publicPort"].asInt());
						
						_socket->connectTo(player2["publicIP"].asString(), player2["publicPort"].asInt(), 2);
						
						usleep(arc4random()%750000);
						if(_socket->isConnected()) {
							loadGameScene();
							break;
						}																								
						CCLOG("Connecting to %s:%d", player2["privateIP"].asCString(), player2["privatePort"].asInt());
						
						_socket->connectTo(player2["privateIP"].asString(), player2["privatePort"].asInt(), 2);
						
						usleep(arc4random()%750000);
						if(_socket->isConnected()) {
							loadGameScene();
							break;
						}
											
					}
					
					
				}else {
					//we are player2 - connect to player1

					//try to connect to both public and private ips in a while loop

					for(int i = 0; i < 50; i++) {
						
						CCLOG("Connecting to %s:%d", player1["publicIP"].asCString(), player1["publicPort"].asInt());
						
						_socket->connectTo(player1["publicIP"].asString(), player1["publicPort"].asInt(), 2);
					
						usleep(arc4random()%750000);
						if(_socket->isConnected()) {
							loadGameScene();
							break;
						}
						
												
						CCLOG("Waiting for connection from %s on port %d", player1["publicIP"].asCString(), _localPort);

						if(!_socket->listenOn(_localPort)) {
							CCLOGERROR("FAILED to Listen on Game Server port %d", _localPort);
						}															
						usleep(arc4random()%750000);
						if(_socket->isConnected()) {
							loadGameScene();
							break;
						}
						_socket->disconnect(false);
						CCLOG("-------Manual disconnect");
	
		

						CCLOG("Connecting to %s:%d", player1["privateIP"].asCString(), player1["privatePort"].asInt());
						
						_socket->connectTo(player1["privateIP"].asString(), player1["privatePort"].asInt(), 2);
						
						usleep(arc4random()%750000);
						if(_socket->isConnected()) {
							loadGameScene();
							break;
						}					
					}
	
				}
				
			}
		}else {
			//TODO: handle errors
			CCLOG("Received messge is an error - ignoring");
			
		}
	}else {
		CCLOG("Recevied message has no 'status' element - ignoring");
	}
}

void LobbyScene::onConnect() {
	CCLOG("Connected to Server");
	
	//identify
	Json::Value user;
	user["userId"] = _userId;
	
	Json::Value identifyMessage;
	identifyMessage["action"] = "identify";
	identifyMessage["user"] = user;
	identifyMessage["privateIP"] = Socket::getLocalIPAddress();
	identifyMessage["privatePort"] = _socket->getLocalPort();
		
	_socket->sendMessage(identifyMessage, false);
	
	//TEST CODE
	//set pref that we're creating a game
		
	Json::Value prefs;
	prefs["playWithFriend"] = _friendUserId;
	
	Json::Value setPrefsMessage;
	setPrefsMessage["action"] = "setPrefs";
	setPrefsMessage["prefs"] = prefs;
	
	_socket->sendMessage(setPrefsMessage, false);
		
}

void LobbyScene::onDisconnect() {
	CCLOG("Disconnected from Server");		

	//TODO: notify user of lost connection
	
}


void LobbyScene::onDisconnectChild() {
	CCLOG("Disconnected Peer");		

	//TODO: notify user of lost connection
	
}


void LobbyScene::onEnter() {
	CCLOG("On Enter LobbyScene");
	this->CCLayer::onEnter();
	
	if(!_socket->connectTo(LOBBY_SERVER, LOBBY_PORT, 5)) {
		//TODO: notify the user
		CCLOGERROR("FAILED to Connect to Lobby Server");
		
	}else {
		_localPort = _socket->getLocalPort();
		CCLOG("Bound on local port %d", _localPort);
		
	}
}

void LobbyScene::loadGameScene() {	
	//let the NetworkedOpponent manage the socket now
	_socket = NULL;
	
	CCDirector::sharedDirector()->replaceScene(CCTransitionFadeBL::create(
		0.5,
		GameScene::scene(_gameScene)
	));
}


void LobbyScene::onExit() {
	CCLOG("On Exit LobbyScene");
	this->CCLayer::onExit();
}



void LobbyScene::touchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

}

void LobbyScene::touchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

}

void LobbyScene::touchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {


}































LobbyScene::~LobbyScene() {
	if(_socket != NULL) {
		delete _socket;
		_socket = NULL;
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

