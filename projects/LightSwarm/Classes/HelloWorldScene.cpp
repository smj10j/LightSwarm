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
	
	_currentViewportScale = VIEWPORT_SCALE_INITIAL;
	_isManipulatingViewport = false;
	_prevViewportManipulationFingerDistance = 0;
	
	
	//create a batch node
	_batchNode = CCSpriteBatchNode::create("Sprites.pvr.ccz");
	this->addChild(_batchNode);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Sprites.plist");
	
	//TODO: seed this with the value we get from the server
	Utilities::setRandomSeed(450);
	
	for(int i = 0; i < 200; i++) {
		//create a ship
		CCSprite* sprite = CCSprite::createWithSpriteFrameName("SpaceFlier_sm_1.png");
		sprite->setPosition(ccp(winSize.width * Utilities::getRandomDouble(), winSize.height * Utilities::getRandomDouble()));
		sprite->setScale(_currentViewportScale*SCALE_FACTOR);
		_batchNode->addChild(sprite, 1);
		
		Spark* spark = new Spark(sprite);
		
		_sparks.insert(spark);
	}
	

	CCLog("Game started at %f", Utilities::getMillis());
	
	this->scheduleUpdate();
	this->setTouchEnabled(true);
	
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
		
	if(touches->count() == 1) {
		//interation with units/orbs
		CCTouch* touch = (CCTouch*)(touches->anyObject());
		CCPoint location = touch->getLocation();

		_prevTouches = _currentTouches;
		_currentTouches.clear();
		_currentTouches.push_back(location);
		
	}else {
		CCLOG("Viewport manipulation");
		//viewport manipulation
		
		CCSetIterator touchIterator = touches->begin();
		CCTouch* touch1 = (CCTouch*)(*touchIterator++);
		CCTouch* touch2 = (CCTouch*)(*touchIterator);		
		
		_currentTouches.clear();
		_isManipulatingViewport = true;
		_prevViewportManipulationFingerDistance = ccpDistance(touch1->getLocation(),
																touch2->getLocation());

	}
}


void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
	
	if(!_isManipulatingViewport && touches->count() > 1) {
		_currentTouches.clear();
		_isManipulatingViewport = true;
	}
	
	if(!_isManipulatingViewport) {
		//interation with units/orbs
		
		if(_currentTouches.size() >= MAX_TOUCHES) {
			return;
		}
		
		CCTouch* touch = (CCTouch*)(touches->anyObject());
		CCPoint location = touch->getLocation();
		
		_currentTouches.push_back(location);
		
	}else if(touches->count() > 1) {
		//viewport manipulation
		
		CCSetIterator touchIterator = touches->begin();
		CCTouch* touch1 = (CCTouch*)(*touchIterator++);
		CCTouch* touch2 = (CCTouch*)(*touchIterator);
		CCPoint location1 = touch1->getLocation();
		CCPoint location2 = touch2->getLocation();
		
		float fingerDistance = ccpDistance(location1, location2);
		CCPoint center = ccp((location1.x+location2.x)/2,
							(location1.y+location2.y)/2);
			
		float fingerDistanceDiff = fingerDistance - _prevViewportManipulationFingerDistance;
		float fingerDistanceDiffPercent = fingerDistanceDiff/_prevViewportManipulationFingerDistance;
		
		if(fabs(fingerDistanceDiffPercent) > 0.15) {
			//pinch
			
			_currentViewportScale+= fingerDistanceDiffPercent;
			if(_currentViewportScale < VIEWPORT_SCALE_MIN) _currentViewportScale = VIEWPORT_SCALE_MIN;
			if(_currentViewportScale > VIEWPORT_SCALE_MAX) _currentViewportScale = VIEWPORT_SCALE_MAX;
			
			for(set<Spark*>::iterator sparksIterator = _sparks.begin();
				sparksIterator != _sparks.end();
				sparksIterator++) {
				
				Spark* spark = *sparksIterator;

				spark->getSprite()->setScale(_currentViewportScale*SCALE_FACTOR);
			}
			
			_prevViewportManipulationFingerDistance = fingerDistance;
			
		}else {
			//drag
			
			
		}
		
	}
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

	CCLOG("Processing %d touches", _currentTouches.size());

	_isManipulatingViewport = false;

	bool isALoop = false;
	bool isStartingWithinExistingLasso = false;
	
	if(!_currentTouches.empty()) {
		isALoop = Utilities::isNear(_currentTouches.front(), _currentTouches.back(), IMMEDIATE_VINCINITY_DISTANCE);
		
		isStartingWithinExistingLasso = Utilities::isPointInShape(_currentTouches.front(), _prevTouches);
	}
		
	if(!_selectedSparks.empty()) {
		//move!
		
		//first let's see if we're drawing a new lasso - we're guessing
		//this by having a start point near the end point
		if(isALoop && !isStartingWithinExistingLasso) {
			//drew a new lasso - even though other sparks are already selected
			_selectedSparks.clear();
			
		}else if(!_currentTouches.empty()) {
		
			if(_currentTouches.size() > 1) {
				//multi touch is valid
						
				CCLog("would now move the selected sparks");
					
				for(set<Spark*>::iterator selectedSparksIterator = _selectedSparks.begin();
					selectedSparksIterator != _selectedSparks.end();
					selectedSparksIterator++) {
					
					(*selectedSparksIterator)->setTargetMovePath(_currentTouches);
				}
				
				_prevTouches = _currentTouches;
			}
			
			_currentTouches.clear();
		}
	}
	
	//remove the selection effects and apply any new efects
	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		sparksIterator++) {

		Spark* spark = *sparksIterator;
			
		if(!_currentTouches.empty()) {
			if(spark->isInShape(_currentTouches)) {
				spark->addSelectionEffect();
				_selectedSparks.insert(spark);
			}else {
				spark->clearAllEffects();
			}
		}else {
			spark->clearAllEffects();
		}
	}
	
	
	if(_currentTouches.empty()) {
		//user tapped somewhere to deselect everything
		_selectedSparks.clear();
		_prevTouches.clear();
		_currentTouches.clear();
	}
}






HelloWorld::~HelloWorld() {

}
