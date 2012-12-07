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
		
	return true;
}


void LobbyScene::onEnter() {
	CCLOG("On Enter LobbyScene");
	this->CCLayer::onEnter();
	
	
	
}

void loadGameScene() {
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

}

