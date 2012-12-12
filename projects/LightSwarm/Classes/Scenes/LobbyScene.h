//
//  LobbyScene.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#ifndef __LightSwarm__LobbyScene__
#define __LightSwarm__LobbyScene__

#include "NetworkedOpponent.h"
USING_NS_CC;
using namespace std;

enum RESPONSE_TYPE {
	CONNECT,
	DISCONNECT,
	MESSAGE
};

#define LOBBY_SERVER	"localhost"
#define LOBBY_PORT		3001

class LobbyScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(LobbyScene);	
	
	//touch handlers
	virtual void ccTouchesMoved(CCSet* touches, CCEvent* event);
	virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	
	virtual void onEnter();
	virtual void onExit();
				
	
	void onConnect();
	void onSuccess(RESPONSE_TYPE type, string message);
	void onError(RESPONSE_TYPE type, string error);
	
	virtual ~LobbyScene();
	
private:
	CCSpriteBatchNode* _batchNode;
	
	
	void loadGameScene();
	
	
	int _sockfd;
		
	bool connectToLobbyServer();
	void disconnectFromLobbyServer();
	void sendMessage(const string& message);

};
			
#endif /* defined(__LightSwarm__LobbyScene__) */
