#include "GameScene.h"
#include "Common.h"
USING_NS_CC;


CCScene* GameScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene *layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }


	//default blend function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	_gameLayer = CCLayer::create();
	this->addChild(_gameLayer);

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	
	_prevViewporCenter = _gameLayer->getPosition();
	_isManipulatingViewport = false;
	
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
	
	for(int i = 0; i < 1000; i++) {
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



void GameScene::update(float dt) {

	/*
	//TEST CODE to simulate a bit of rollback
	if(_currentRunningTime > 10) {
		singleUpdateStep(-dt);
		return;
	}
	*/
	
	_currentRunningTime+= dt;
	_fixedTimestepAccumulator+= dt;

	if(_isRollingBack) {
		//new command was received from network player - adding it in
		return;
	}
	
	const double stepSize = Config::getDoubleForKey(SIMULATION_STEP_SIZE);
	const int maxSteps = Config::getDoubleForKey(SIMULATION_MAX_STEPS);
	const int steps = _fixedTimestepAccumulator / stepSize;
	
	if (steps > 0) {
        _fixedTimestepAccumulator-= (steps * stepSize);

		const int stepsClamped = MIN(steps, maxSteps);
	 
		for (int i = 0; i < stepsClamped; i++) {
			singleUpdateStep(stepSize);
		}
	}else {
		//no step - we're just too dang fast!
	}
}


/* Use a negative step (-dt) on opponents sparks to rollback their position to the time of a command
then apply the command and turn off rollback - the simulation should catch up automaticaly
*/
void GameScene::singleUpdateStep(float dt) {

	//update sparks
	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		 ) {
				
		Spark* spark = *sparksIterator;
		
		if(spark->isDead()) {
			//TODO: how to handle this with rollback?
			//make the spark invisible for a few second before finally removing it?
			(*sparksIterator)->remove();
			_sparks.erase(sparksIterator++);
			continue;
		}
		
		spark->setNearestOrb(_orbs);
		spark->update(dt);
		
		sparksIterator++;
	}
	
	//update orbs
	for(set<Orb*>::iterator orbsIterator = _orbs.begin();
		orbsIterator != _orbs.end();
		 orbsIterator++) {
				
		Orb* orb = *orbsIterator;
		
		orb->update(dt);
	}
	
	
	
	if(dt > 0) {
		
		//things in here don't need to listen to rollback
	
		for(list<PingLocation*>::iterator pingLocationsIterator = _pingLocations.begin();
			pingLocationsIterator != _pingLocations.end();
			pingLocationsIterator++) {
			(*pingLocationsIterator)->update(dt);
		}
	}
}


void GameScene::draw() {
	
	if(_currentTouches.size() >= 2) {
		//draw a line as we drag our finger
		glLineWidth(5);
		ccDrawColor4B(255, 255, 255, 255);
		CCPoint offset = _gameLayer->convertToWorldSpace(CCPointZero);
		list<CCPoint>::iterator prevcurrentTouchesIterator = _currentTouches.begin();
		for(list<CCPoint>::iterator currentTouchesIterator = prevcurrentTouchesIterator;
			currentTouchesIterator != _currentTouches.end();
			prevcurrentTouchesIterator = currentTouchesIterator++) {
				
			ccDrawLine(ccpAdd(*prevcurrentTouchesIterator, offset),
						ccpAdd(*currentTouchesIterator, offset)
			);
		}
	}
	

}



void GameScene::clearPingLocations() {
	for(list<PingLocation*>::iterator pingLocationsIterator = _pingLocations.begin();
		pingLocationsIterator != _pingLocations.end();
		_pingLocations.erase(pingLocationsIterator++)) {
		this->removeChild(*pingLocationsIterator, true);
	}
}

void GameScene::clearSelectedSparksIfNoAction() {
	if(!_isManipulatingSparks && !_isManipulatingViewport) {
		_selectedSparks.clear();
		clearPingLocations();
		updateSparkSelectionEffects();
	}
}

void GameScene::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
		
	if(touches->count() != 1) { return; };

	CCTouch* touch = (CCTouch*)touches->anyObject();
	CCPoint location = touch->getLocation();
	CCPoint mapLocation = ccpSub(location, _gameLayer->convertToWorldSpace(CCPointZero));

	_currentTouches.clear();
	clearPingLocations();

	double now = Utilities::getMillis();
	if(now - _lastTouchBeganMillis <= Config::getIntForKey(TOUCH_DOUBLE_TAP_DELAY_MILLIS)) {
		//double-tap!
		_isManipulatingViewport = false;
		_isManipulatingSparks = true;
		_currentTouches.push_back(mapLocation);
		
	}else {
		//normal tap
		
		_prevViewporCenter = location;
		_isManipulatingViewport = false;
		_isManipulatingSparks = false;
		
		//ping!
		if(_selectedSparks.empty()) {
			PingLocation* pingLocation = new PingLocation(location,
													1,
													SCALE_FACTOR*150,
													ccc4(0, 0, 255, 255));
			_pingLocations.push_back(pingLocation);
			this->addChild(pingLocation);
		}else {
			PingLocation* pingLocation = new PingLocation(location,
													1,
													SCALE_FACTOR*150,
													ccc4(0, 255, 0, 255));
			_pingLocations.push_back(pingLocation);
			this->addChild(pingLocation);
		}
		
		this->scheduleOnce(schedule_selector(GameScene::clearSelectedSparksIfNoAction), Config::getIntForKey(TOUCH_DOUBLE_TAP_DELAY_MILLIS)/1000.0+0.100);
	}
	
	_lastTouchBeganMillis = now;
}


void GameScene::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
	
	if(touches->count() != 1) { return; };
	
	CCTouch* touch = (CCTouch*)touches->anyObject();
	CCPoint location = touch->getLocation();
	CCPoint mapLocation = ccpSub(location, _gameLayer->convertToWorldSpace(CCPointZero));

				
	if(!_isManipulatingSparks && !_isManipulatingViewport) {
		//well by golly, what ARE we doing?
		
		double now = Utilities::getMillis();
		list<CCPoint> sparkPositions = Spark::getPositionList(_selectedSparks);

		if(!_selectedSparks.empty() && (
				(now - _lastTouchBeganMillis) >= Config::getDoubleForKey(TOUCH_MOVE_BEGAN_DELAY_MILLIS) ||
				Utilities::isNear(mapLocation, sparkPositions, NEARBY_DISTANCE) ||
				Utilities::isPointInShape(mapLocation, _prevTouches))
			) {
			//press and hold, or placed finger in selecting lasso or near selected sparks
			_isManipulatingViewport = false;
			_isManipulatingSparks = true;
						
		}else if((now - _lastTouchBeganMillis) >= Config::getDoubleForKey(TOUCH_LASSO_BEGAN_DELAY_MILLIS)) {
				//started a drag movement by holding a finger down
				//or restarted a drag by touching the screen while the view is still sliding
				_isManipulatingViewport = false;
				_isManipulatingSparks = true;
					
		}else {
			_isManipulatingViewport = true;
			_isManipulatingSparks = false;
			clearPingLocations();
		}
	}
	
	if(_isManipulatingSparks) {
		//interation with units/orbs
		
		if(_currentTouches.size() >= Config::getIntForKey(MAX_TOUCHES)) {
			return;
		}
		
		double distance = _currentTouches.empty() ? 100000 : Utilities::getDistance(mapLocation, _currentTouches.back());
		
		if(distance >= Config::getIntForKey(TOUCH_MIN_PATH_POINT_DISTANCE)) {
			_currentTouches.push_back(mapLocation);
		}
		
	}else if(_isManipulatingViewport) {
		//viewport manipulation
		
		_gameLayer->stopAllActions();
		
		CCPoint dragDiff = ccpSub(location, _prevViewporCenter);
			
		if(ccpLength(dragDiff) > 5) {
			//drag
			
			if(!isnan(dragDiff.x) && !isnan(dragDiff.y)) {
				_gameLayer->setPosition(ccpAdd(_gameLayer->getPosition(), dragDiff));
				_viewportDragVelocity = dragDiff;
			}
			
		}
		
		_prevViewporCenter = location;
	}
}

void GameScene::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

	if(touches->count() != 1) { return; };

	_lastTouchEndedMillis = Utilities::getMillis();
	
	if(_isManipulatingViewport) {
			
		_gameLayer->stopAllActions();
						
		_gameLayer->runAction(CCSequence::create(
			CCMoveBy::create(0.15, ccpMult(_viewportDragVelocity, 3)),
			CCMoveBy::create(0.20, ccpMult(_viewportDragVelocity, 2)),
			CCMoveBy::create(0.25, ccpMult(_viewportDragVelocity, 1)),
			NULL
		));
		
		_isManipulatingViewport = false;
		_isManipulatingSparks = false;
		return;
		
	}else if(!_selectedSparks.empty() && !_currentTouches.empty()){
		CCPoint offset = _gameLayer->convertToWorldSpace(CCPointZero);
		PingLocation* pingLocation = new PingLocation(ccpAdd(_currentTouches.back(),offset),
												1,
												SCALE_FACTOR*150,
												ccc4(255, 0, 0, 255));
		_pingLocations.push_back(pingLocation);
		this->addChild(pingLocation);
	}
	_isManipulatingViewport = false;
	_isManipulatingSparks = false;

	if(_currentTouches.empty()) {
		return;
	}

	bool isALoop = _currentTouches.size() > 1 && Utilities::isNear(_currentTouches.front(), _currentTouches.back(), NEARBY_DISTANCE+Config::getIntForKey(TOUCH_MIN_PATH_POINT_DISTANCE));
	bool isStartingWithinExistingLasso = Utilities::isPointInShape(_currentTouches.front(), _prevTouches);
					
	if(!_selectedSparks.empty()) {
		//move!
		
		//first let's see if we're drawing a new lasso - we're guessing
		//this by having a start point near the end point
		if(isALoop && !isStartingWithinExistingLasso) {
			//drew a new lasso - even though other sparks are already selected
			_selectedSparks.clear();
			clearPingLocations();
			_prevTouches = _currentTouches;
			
		}else {
				
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
				
				(*selectedSparksIterator)->setTargetMovePath(_currentTouches);
			}
				
			_prevTouches = _currentTouches;
			_currentTouches.clear();
			_selectedSparks.clear();
		}
	}else if(!_currentTouches.empty()) {

		if(!isALoop) {
			//complete the loop
			_currentTouches.push_back(_currentTouches.front());
		}	
	
		_prevTouches = _currentTouches;
	}

	updateSparkSelectionEffects();
}

void GameScene::updateSparkSelectionEffects() {
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




GameScene::~GameScene() {
	clearPingLocations();
}
