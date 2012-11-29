#include "HelloWorldScene.h"
#include "Common.h"
USING_NS_CC;


CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	
	
	
	
	//create a batch node
	_batchNode = CCSpriteBatchNode::create("Sprites.pvr.ccz");
	this->addChild(_batchNode);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Sprites.plist");
	 
	//create a ship
	_ship = CCSprite::createWithSpriteFrameName("SpaceFlier_sm_1.png");
	_ship->setPosition(ccp(winSize.width * 0.1, winSize.height * 0.5));
	_batchNode->addChild(_ship, 1);
		
	//add some stars
	CCParticleSystemQuad* stars1 = CCParticleSystemQuad::create("Stars1.plist");
	stars1->setSourcePosition(ccp(winSize.width * 0.1, winSize.height * 0.5 - stars1->getContentSize().height));
	this->addChild(stars1);
	
	this->setTouchEnabled(true) ;
	
	CCLog("Game started at %f", Utilities::getMillis());
	
	return true;
}


void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize() ;
	
	CCTouch* touch = (CCTouch*)(touches->anyObject());
	CCPoint location = touch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	
	CCLog("touch at %f", Utilities::getMillis());
	CCLOG("got touch at %f,%f", location.x, location.y);
}
