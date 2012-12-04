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
	
	_gameLayer = CCLayer::create();
	this->addChild(_gameLayer);

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	
	_currentViewportScale = Config::getIntForKey(CONFIG_VIEWPORT_SCALE_INITIAL);
	_prevViewporCenter = _gameLayer->getPosition();
	_isManipulatingViewport = false;
	_prevViewportManipulationFingerDistance = 0;
	
	//create a batch node
	_batchNode = CCSpriteBatchNode::create("Sprites.pvr.ccz");
	_gameLayer->addChild(_batchNode);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Sprites.plist");
	
	//TODO: seed this with the value we get from the server
	Utilities::setRandomSeed(450);
	
	
	for(int i = 0; i < 40; i++) {
		//create an orb
		CCSprite* sprite = CCSprite::createWithSpriteFrameName("asteroid.png");
		sprite->setPosition(ccp(winSize.width * (3*Utilities::getRandomDouble()-1.5), winSize.height * (3*Utilities::getRandomDouble()-1.5)));
		sprite->setScale(SCALE_FACTOR*(Utilities::getRandomDouble()*3));
		_batchNode->addChild(sprite, 1);
		
		Orb* orb = new Orb(sprite);
		
		_orbs.insert(orb);
	}	
	
	for(int i = 0; i < 500; i++) {
		//create a ship
		CCSprite* sprite = CCSprite::createWithSpriteFrameName("SpaceFlier_sm_1.png");
		sprite->setPosition(ccp(winSize.width * (3*Utilities::getRandomDouble()-1.5), winSize.height * (3*Utilities::getRandomDouble()-1.5)));
		sprite->setScale(SCALE_FACTOR);
		_batchNode->addChild(sprite, 1);
		
		Spark* spark = new Spark(sprite, 1.0, 1.0, 1.0);
		
		_sparks.insert(spark);
	}
	

	CCLog("Game started at %f", Utilities::getMillis());
	
	this->scheduleUpdate();
	this->setTouchEnabled(true);
		
	return true;
}



void HelloWorld::update(float dt) {
				
	//update sparks
	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		 ) {
				
		Spark* spark = *sparksIterator;
		
		if(spark->isDead()) {
			_sparks.erase(sparksIterator++);
			continue;
		}
		
		Orb* nearestOrb = NULL;
		float minDistance = 10000000;
		for(set<Orb*>::iterator orbsIterator = _orbs.begin();
			orbsIterator != _orbs.end();
			orbsIterator++) {
			float distance = ccpDistance((*orbsIterator)->getSprite()->getPosition(), spark->getSprite()->getPosition());
			if(distance < minDistance) {
				minDistance = distance;
				nearestOrb = (*orbsIterator);
			}
		}
		
		spark->update(nearestOrb, dt);
		
		sparksIterator++;
	}
	
	//update orbs
	for(set<Orb*>::iterator orbsIterator = _orbs.begin();
		orbsIterator != _orbs.end();
		 orbsIterator++) {
				
		Orb* orb = *orbsIterator;
		
		orb->update(dt);
	}	
}


void HelloWorld::draw() {
	
	if(_currentTouches.size() < 5) {
		//don't draw yet
		return;
	}
	
	//draw a line as we drag our finger
	glLineWidth(5);
	list<CCPoint>::iterator prevcurrentTouchesIterator = _currentTouches.begin();
	for(list<CCPoint>::iterator currentTouchesIterator = _currentTouches.begin();
		currentTouchesIterator != _currentTouches.end();
		prevcurrentTouchesIterator = currentTouchesIterator++) {
			
		ccDrawLine(*prevcurrentTouchesIterator,
					*currentTouchesIterator
		);
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
		//viewport manipulation
		
		CCSetIterator touchIterator = touches->begin();
		CCTouch* touch1 = (CCTouch*)(*touchIterator++);
		CCTouch* touch2 = (CCTouch*)(*touchIterator);
		CCPoint location1 = touch1->getLocation();
		CCPoint location2 = touch2->getLocation();
		
		
		_currentTouches.clear();
		_isManipulatingViewport = true;
		_prevViewportManipulationFingerDistance = ccpDistance(touch1->getLocation(),
																touch2->getLocation());
		_prevViewporCenter = ccpMidpoint(location1, location2);

	}
}


void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
	
		
	CCSetIterator touchIterator = touches->begin();
	CCTouch* touch1 = (CCTouch*)(*touchIterator);
	CCTouch* touch2 = (CCTouch*)(touches->count() > 1 ? *(++touchIterator) : *touchIterator);
	CCPoint location1 = touch1->getLocation();
	CCPoint location2 = touch2->getLocation();
	
	
	if(!_isManipulatingViewport && touches->count() > 1) {
		//added another finger
		_currentTouches.clear();
		_prevViewportManipulationFingerDistance = ccpDistance(touch1->getLocation(),
															  touch2->getLocation());
		_prevViewporCenter = ccpMidpoint(location1, location2);
		_isManipulatingViewport = true;
	}
	
	if(!_isManipulatingViewport) {
		//interation with units/orbs
		
		if(_currentTouches.size() >= Config::getIntForKey(CONFIG_MAX_TOUCHES)) {
			return;
		}
		
		CCTouch* touch = (CCTouch*)(touches->anyObject());
		CCPoint location = touch->getLocation();
		
		_currentTouches.push_back(location);
		
	}else if(touches->count() > 1) {
		//viewport manipulation
		
		float fingerDistance = ccpDistance(location1, location2);
		CCPoint center = ccpMidpoint(location1, location2);
		CCPoint dragDiff = ccpSub(center, _prevViewporCenter);
			
		float fingerDistanceDiff = fingerDistance - _prevViewportManipulationFingerDistance;
		float fingerDistanceDiffPercent = fingerDistanceDiff/_prevViewportManipulationFingerDistance;
		
		if(ccpDistance(center, _prevViewporCenter) > 5) {
			//drag
			
			if(!isnan(dragDiff.x) && !isnan(dragDiff.y)) {
				_gameLayer->setPosition(ccpAdd(_gameLayer->getPosition(), dragDiff));
			}
			
		}/*else if(fabs(fingerDistanceDiffPercent) > 0.15) {
			//pinch
					
			if(fingerDistanceDiffPercent < 1) {				
				_currentViewportScale+= fingerDistanceDiffPercent;
				if(_currentViewportScale < Config::getIntForKey(CONFIG_VIEWPORT_SCALE_MIN)) _currentViewportScale = Config::getIntForKey(CONFIG_VIEWPORT_SCALE_MIN);
				if(_currentViewportScale > Config::getIntForKey(CONFIG_VIEWPORT_SCALE_MAX)) _currentViewportScale = Config::getIntForKey(CONFIG_VIEWPORT_SCALE_MAX);
				
				for(set<Spark*>::iterator sparksIterator = _sparks.begin();
					sparksIterator != _sparks.end();
					sparksIterator++) {
					
					Spark* spark = *sparksIterator;

					spark->setViewportScale(_currentViewportScale);
				}
					
				if(!isnan(dragDiff.x) && !isnan(dragDiff.y)) {
					_gameLayer->setPosition(ccpAdd(_gameLayer->getPosition(), ccpMult(dragDiff, 1/_currentViewportScale)));
				}				
				
			}else {
				//error from slow fingers
			}
			
			_prevViewportManipulationFingerDistance = fingerDistance;
			
		}*/
		
		_prevViewporCenter = center;
	}
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {


	if(_isManipulatingViewport || _currentTouches.empty()) {
		_isManipulatingViewport = false;
		return;
	}
	_isManipulatingViewport = false;

	CCLOG("Processing %d touches", _currentTouches.size());

	bool isALoop = false;
	bool isStartingWithinExistingLasso = false;
	
	if(!_currentTouches.empty()) {
		isALoop = Utilities::isNear(_currentTouches.front(), _currentTouches.back(), IMMEDIATE_VINCINITY_DISTANCE);
		
		isStartingWithinExistingLasso = Utilities::isPointInShape(_currentTouches.front(), _prevTouches);
	}else {
		//user tapped somewhere to deselect everything
		_selectedSparks.clear();
		_prevTouches.clear();
		_currentTouches.clear();
		return;
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
				
				//remove touches within the starting lasso if our ending touch is outside of it
				if(!Utilities::isPointInShape(_currentTouches.back(), _prevTouches)) {
					for(list<CCPoint>::iterator currentTouchesIterator = _currentTouches.begin();
						currentTouchesIterator != _currentTouches.end();
						_currentTouches.erase(currentTouchesIterator++)) {
						if(!Utilities::isPointInShape(*currentTouchesIterator, _prevTouches)) {
							break;
						}
					}
				}
																
				for(set<Spark*>::iterator selectedSparksIterator = _selectedSparks.begin();
					selectedSparksIterator != _selectedSparks.end();
					selectedSparksIterator++) {
					
					(*selectedSparksIterator)->setTargetMovePath(_currentTouches, _gameLayer->convertToWorldSpace(CCPointZero));
				}
				
				_prevTouches = _currentTouches;
			}
			
			_currentTouches.clear();
			_selectedSparks.clear();
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
}






HelloWorld::~HelloWorld() {

}
