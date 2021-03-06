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
#include "GameScene.h"

USING_NS_CC;
using namespace std;

#define LOBBY_SERVER	"162.243.49.135"
#define LOBBY_PORT		3001

class LobbyScene : public Layer, public SocketDelegate
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(LobbyScene);	
	
	//touch handlers
	virtual void touchesMoved(CCSet* touches, CCEvent* event);
	virtual void touchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	virtual void touchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	
	virtual void onEnter();
	virtual void onExit();
	
	virtual void onConnect();
	virtual void onDisconnect();
	virtual void onDisconnectChild();
	virtual void onMessage(const Json::Value& message);
		
	virtual ~LobbyScene();
	
private:
	CCSpriteBatchNode* _batchNode;
	
	GameScene* _gameScene;
	
	
	void loadGameScene();
	
	string _userId;
	string _friendUserId;
	Socket* _socket;
	int _localPort;
};
			
#endif /* defined(__LightSwarm__LobbyScene__) */
