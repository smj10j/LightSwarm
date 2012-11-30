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
	
	//TODO: seed this with the value we get from the server
	_randDouble.seed(450);
	
	for(int i = 0; i < 50; i++) {
		//create a ship
		CCSprite* sprite = CCSprite::createWithSpriteFrameName("SpaceFlier_sm_1.png");
		sprite->setPosition(ccp(winSize.width * _randDouble(), winSize.height * _randDouble()));
		_batchNode->addChild(sprite, 1);
		
		Spark* spark = new Spark(sprite);
		
		_sparks.insert(spark);
	}
	
	this->setTouchEnabled(true);

	CCLog("Game started at %f", Utilities::getMillis());
	
	this->scheduleUpdate();
	
	return true;
}



void HelloWorld::update(CCTime dt) {


}


void HelloWorld::draw() {
	
	//draw a line as we drag our finger
	glLineWidth(10);
	list<CCPoint>::iterator prevLassoTouchesIterator = _lassoTouches.begin();
	for(list<CCPoint>::iterator lassoTouchesIterator = _lassoTouches.begin();
		lassoTouchesIterator != _lassoTouches.end();
		prevLassoTouchesIterator = lassoTouchesIterator++) {
			
		ccDrawLine(*prevLassoTouchesIterator, *lassoTouchesIterator);
	}
	
}




void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
	_lassoTouches.clear();
	_selectedSparks.clear();
}


void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize() ;
	
	CCTouch* touch = (CCTouch*)(touches->anyObject());
	CCPoint location = touch->getLocation();

	CCLog("touch at %f", Utilities::getMillis());
	CCLOG("got touch at %f,%f", location.x, location.y);
	
	_lassoTouches.push_back(location);
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

	CCLOG("Processing %d touches", _lassoTouches.size());
	
	//add a few to test
	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		sparksIterator++) {

		Spark* spark = *sparksIterator;
		
		spark->clearAllEffects();
	
		if(spark->isInShape(_lassoTouches)) {
			spark->addGlowEffect(ccc3(255,255,255), CCSizeMake(2.5f, 2.5f));
			_selectedSparks.insert(spark);
		}
	}
	
}






HelloWorld::~HelloWorld() {

}
