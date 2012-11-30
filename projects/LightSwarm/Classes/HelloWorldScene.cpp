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



void HelloWorld::update(float dt) {

	//let each spark update itself
	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		sparksIterator++) {
		
		(*sparksIterator)->update(dt);
	}
}


void HelloWorld::draw() {
	
	//draw a line as we drag our finger
	glLineWidth(5);
	list<CCPoint>::iterator prevcurrentTouchesIterator = _currentTouches.begin();
	for(list<CCPoint>::iterator currentTouchesIterator = _currentTouches.begin();
		currentTouchesIterator != _currentTouches.end();
		prevcurrentTouchesIterator = currentTouchesIterator++) {
			
		ccDrawLine(*prevcurrentTouchesIterator, *currentTouchesIterator);
	}
	
}




void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

	_currentTouches.clear();
	
}


void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize() ;
	
	CCTouch* touch = (CCTouch*)(touches->anyObject());
	CCPoint location = touch->getLocation();

	CCLog("touch at %f", Utilities::getMillis());
	CCLOG("got touch at %f,%f", location.x, location.y);
	
	_currentTouches.push_back(location);
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

	CCLOG("Processing %d touches", _currentTouches.size());
	
	if(!_selectedSparks.empty()) {
		//move!
		
		CCLog("would now move the selected sparks");
			
		for(set<Spark*>::iterator selectedSparksIterator = _selectedSparks.begin();
			selectedSparksIterator != _selectedSparks.end();
			selectedSparksIterator++) {
			
			(*selectedSparksIterator)->setTargetMovePath(_currentTouches);
		}
		
		_currentTouches.clear();
	}
	
	//remove the selection effects and apply any new efects
	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		sparksIterator++) {

		Spark* spark = *sparksIterator;
		
		spark->clearAllEffects();
	
		if(!_currentTouches.empty()) {
			if(spark->isInShape(_currentTouches)) {
				spark->addGlowEffect(ccc3(255,255,255), CCSizeMake(2.5f, 2.5f));
				_selectedSparks.insert(spark);
			}
		}
	}
	
	
	if(_currentTouches.empty()) {
		//user tapped somewhere to deselect everything
		_selectedSparks.clear();
		_currentTouches.clear();
	}
}






HelloWorld::~HelloWorld() {

}
