#include "GameScene.h"
#include "GameStateSnapshot.h"
#include "Command.h"
USING_NS_CC;


CCScene* GameScene::scene(Player* player, Opponent* opponent) {
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameScene *layer = GameScene::create();
	
	layer->_player = player;
	layer->_opponent = opponent;
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

CCScene* GameScene::scene(GameScene *layer) {
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
	
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init() {
    if ( !CCLayer::init() ) {
        return false;
    }

	_gameLayer = CCLayer::create();
	this->addChild(_gameLayer);

	CCSize winSize = CCDirector::getInstance()->getWinSize();
	
	_prevViewporCenter = _gameLayer->getPosition();
	_isManipulatingViewport = false;
	_isManipulatingSparks = false;
	_isRestoringGameStateSnapshot = false;
	_isCreatingGameStateSnapshot = false;
	
	_currentFrame = 0;
	
	//create a batch node
	_batchNode = CCSpriteBatchNode::create("Sprites.png");
	_gameLayer->addChild(_batchNode);
	CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile("Sprites.plist");
	
	
	//TODO: create a clientSocket or serverSocket depending on isServer variable
	
	//TODO: seed this with the value we get from the server
	Utilities::setRandomSeed(450);
	
	
	for(int i = 0; i < 40; i++) {
		//create an orb

		CCPoint position = ccp(winSize.width * (3*Utilities::getRandomDouble()-1.5), winSize.height * (3*Utilities::getRandomDouble()-1.5));
		
		Orb* orb = new Orb(_batchNode, position, Utilities::getRandomDouble()*2+.5);
		orb->addSpriteToParent();
		
		_orbs.push_back(orb);
	}	
	
	for(int i = 0; i < 1000; i++) {
		//create a ship
		
		CCPoint position = ccp(winSize.width * (3*Utilities::getRandomDouble()-1.5), winSize.height * (3*Utilities::getRandomDouble()-1.5));
		
		Spark* spark = new Spark(_batchNode, position, 1.0, 1.0, 1.0, 1.0);
		spark->addSpriteToParent();
		
		_sparks.push_back(spark);
	}
	

	CCLog("Game started at %f", Utilities::getMillis());
	
	this->scheduleUpdate();
	this->setTouchEnabled(true);
		
	return true;
}

void GameScene::onEnter() {
	this->CCLayer::onEnter();
	CCLOG("On Enter GameScene");
	
	
	for(int i = 0; i < 5; i++) {
		sleep(3);

		//TODO: test message on connect
		Json::Value supMessage;
		supMessage["action"] = "say sup";
		supMessage["message"] = "sup";
		
		//TODO: WHY IS THIS NEVER RECEIVED???
		((NetworkedOpponent*)_opponent)->_socket->sendMessage(supMessage);
		CCLOG("Trying to send a sup message to peer");
	}
}

void GameScene::onExit() {
	this->CCLayer::onExit();
	CCLOG("On Exit GameScene");
}

void GameScene::update(float dt) {

	if(_isRestoringGameStateSnapshot) return;
	
	//NOTICE: TEST CODE to simulate a bit of rollback
	static bool HAS_TESTED_ROLLBACK = false;
	if(!HAS_TESTED_ROLLBACK && _currentFrame == 500) {
		HAS_TESTED_ROLLBACK = true;
		list<int> ids;
		for(int i = 0; i < 200; i++) {
			ids.push_back(i);
		}
		list<CCPoint> path;
		path.push_back(ccp(400,400));
		_commandQueue.push_back(new Command(MOVE, _currentFrame-100, SPARK, ids, path));
	}
		
	_fixedTimestepAccumulator+= dt;
	
	//dynamically set this guy
	const int BASELINE_MAX_STEPS = Config::getDoubleForKey(SIMULATION_MAX_STEPS);
	static float maxSteps = BASELINE_MAX_STEPS;
	static float lastFixedTimestepAccumulator = _fixedTimestepAccumulator;
	
	const double stepSize = Config::getDoubleForKey(SIMULATION_STEP_SIZE);
	const int steps = _fixedTimestepAccumulator / stepSize;
		
	if (steps > 0) {

		const int stepsClamped = MIN(steps, (int)maxSteps);
        _fixedTimestepAccumulator-= (stepsClamped * stepSize);
	 
		for (int i = 0; i < stepsClamped; i++) {
			singleUpdateStep(stepSize);
		}
	}else {
		//no step - we're just too dang fast!
	}
	
	//dynamically adjust maxSteps
	if(_fixedTimestepAccumulator > lastFixedTimestepAccumulator) {
		maxSteps = MIN(maxSteps+.1, BASELINE_MAX_STEPS);
	}else if(_fixedTimestepAccumulator < lastFixedTimestepAccumulator) {
		maxSteps = MAX(maxSteps-.5, 2);
	}
	//CCLOG("maxSteps = %d, lastFixedTimestepAccumulator = %f, _fixedTimestepAccumulator = %f", maxSteps, lastFixedTimestepAccumulator, _fixedTimestepAccumulator);
	lastFixedTimestepAccumulator = _fixedTimestepAccumulator;
	
	processCommandQueue();
}

void GameScene::processCommandQueue() {

	for(list<Command*>::iterator commandQueueIterator = _commandQueue.begin();
		commandQueueIterator != _commandQueue.end();
		_commandQueue.erase(commandQueueIterator++)) {
		
		processCommand(*commandQueueIterator);
		
		delete *commandQueueIterator;
	}

}

//TODO: can we lose commands if we receive 2 in rapid succession?
//will the second rollback erase any changes made by the first command?
//should we take a snapshot after every command?
void GameScene::processCommand(Command* command) {

	CCLOG("_currentFrame = %d command frame = %d", _currentFrame, command->_frame);

	if(_currentFrame != command->_frame) {

		//requires a restore - probably remote

		_isRestoringGameStateSnapshot = true;

		const double stepSize = Config::getDoubleForKey(SIMULATION_STEP_SIZE);

		//store state
		const int prevFrame = _currentFrame;
		list<int> selectedSparkIds;
		for(set<Spark*>::iterator selectedSparksIterator = _selectedSparks.begin();
			selectedSparksIterator != _selectedSparks.end();
			selectedSparksIterator++) {
			selectedSparkIds.push_back((*selectedSparksIterator)->getId());
		}

		CCLOG("Received command to restore to frame %d (we're at %d - diff = %d frames, which is about %fms in the %s)", command->_frame, _currentFrame, (command->_frame - _currentFrame), abs(command->_frame - _currentFrame)*Config::getDoubleForKey(SIMULATION_STEP_SIZE)*1000, command->_frame > _currentFrame ? "future" : "past");
		
		//target frame is in the future - let's catch up!
		while(_currentFrame < command->_frame) {
			singleUpdateStep(stepSize);
		}

		//target frame is in the past
		if(command->_frame < _currentFrame) {
			GameStateSnapshot* gameStateSnapshot = NULL;
			while(!_gameStateSnapshots.empty()) {
				gameStateSnapshot = _gameStateSnapshots.front();
				_gameStateSnapshots.pop_front();
				CCLOG("Examining if we should rollback to frame %d", gameStateSnapshot->_frame);
				if(gameStateSnapshot->_frame <= command->_frame) {
					CCLOG("ROLLBACK MATCH!");
					break;
				}
			}
			
			if(gameStateSnapshot != NULL) {
			
				clearPingLocations();

				//restore to a snapshot
				gameStateSnapshot->restoreTo(this);
				
				//catch up to our target frame
				while(_currentFrame < command->_frame) {
					singleUpdateStep(stepSize);
				}
			}
		}

		//TODO: some sparks that are already moving are not showing a selection effect but are actually selected???
		//reselect sparks
		_selectedSparks.clear();
		for(list<int>::iterator selectedSparkIdsIterator = selectedSparkIds.begin();
			selectedSparkIdsIterator != selectedSparkIds.end();
			selectedSparkIdsIterator++) {
			for(list<Spark*>::iterator sparksIterator = _sparks.begin();
				sparksIterator != _sparks.end();
				sparksIterator++) {
				if((*selectedSparkIdsIterator) == (*sparksIterator)->getId()) {				
					_selectedSparks.insert(*sparksIterator);
					break;
				}
			}
		}
		updateSparkSelectionEffects();	
		
		CCLOG("Applying commands at frame %d with %d frames to fast-forward through", _currentFrame, (prevFrame - _currentFrame));
		
		executeCommand(command);

		//catch up to our previous frame
		/*
		while(_currentFrame < prevFrame) {
			singleUpdateStep(stepSize);
		}
		CCLOG("We are now at frame %d", _currentFrame);
		*/
		if(_currentFrame < prevFrame) {
			_fixedTimestepAccumulator = (prevFrame - _currentFrame) * stepSize;
		}
					
		_isRestoringGameStateSnapshot = false;
		
	}else {
		//no restore required - probably local
		executeCommand(command);
		
	}
}

void GameScene::executeCommand(Command* command) {
	
	CCLOG("Executing command %s on %d %ss with target frame %d current frame %d",
		command->_command == MOVE ? "MOVE" : "UNKNOWN",
		command->_ids.size(),
		command->_idType == SPARK ? "SPARK" : "ORB",
		command->_frame,
		_currentFrame
	);
	
	//TODO: handle more than just the MOVE SPARK command
	
	if(command->_idType == SPARK) {
		for(list<int>::iterator idsIteratore = command->_ids.begin();
			idsIteratore != command->_ids.end();
			idsIteratore++) {
			for(list<Spark*>::iterator sparksIterator = _sparks.begin();
				sparksIterator != _sparks.end();
				sparksIterator++) {
				if((*idsIteratore) == (*sparksIterator)->getId()) {				
					if(command->_command == MOVE) {
						(*sparksIterator)->setTargetMovePath(command->_path);
					}
					break;
				}
			}
		}
	}
	
	
}


void GameScene::singleUpdateStep(float dt) {

	//update sparks
	for(list<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		 ) {
				
		Spark* spark = *sparksIterator;
		
		if(spark->isDead()) {
			deselectSpark(*sparksIterator);
			delete *sparksIterator;
			_sparks.erase(sparksIterator++);
			continue;
		}
		
		spark->setNearestOrb(_orbs);
		spark->update(dt);
		
		sparksIterator++;
	}
	
	//update orbs
	for(list<Orb*>::iterator orbsIterator = _orbs.begin();
		orbsIterator != _orbs.end();
		 orbsIterator++) {
				
		Orb* orb = *orbsIterator;
		
		orb->update(dt);
	}
	
	
	
	
	for(list<PingLocation*>::iterator pingLocationsIterator = _pingLocations.begin();
		pingLocationsIterator != _pingLocations.end();
		pingLocationsIterator++) {
		(*pingLocationsIterator)->update(dt);
	}
	
	
	//see if we need to take a game state snapshot
	if(_currentFrame % Config::getIntForKey(SIMULATION_FRAME_SNAPSHOT_INTERVAL) == 0
		&& !_isCreatingGameStateSnapshot) {
		_isCreatingGameStateSnapshot = true;
		this->scheduleOnce(schedule_selector(GameScene::createGameStateSnapshot), 0.01);
	}
	
	
	_currentFrame++;	
}

void GameScene::createGameStateSnapshot(float dt) {
	if(_gameStateSnapshots.size() >= Config::getIntForKey(SIMULATION_FRAME_STACK_SIZE)) {
		//TODO: optimize this so we're not deleting so often - delete in bulk?
		GameStateSnapshot* gameStateSnapshot = _gameStateSnapshots.back();
		_gameStateSnapshots.pop_back();
		delete gameStateSnapshot;
	}
	_gameStateSnapshots.push_front(new GameStateSnapshot(this));
	_isCreatingGameStateSnapshot = false;
}


void GameScene::draw() {
	
	if(_currentTouches.size() >= 2) {
		//draw a line as we drag our finger
		glLineWidth(5);
		DrawPrimitives::setDrawColor4B(255, 255, 255, 255);
        Point offset = _gameLayer->convertToWorldSpace(Point::ZERO);
		list<Point>::iterator prevcurrentTouchesIterator = _currentTouches.begin();
		for(list<Point>::iterator currentTouchesIterator = prevcurrentTouchesIterator;
			currentTouchesIterator != _currentTouches.end();
			prevcurrentTouchesIterator = currentTouchesIterator++) {
				
			DrawPrimitives::drawLine(*prevcurrentTouchesIterator + offset, *currentTouchesIterator + offset);
		}
	}
	

}



void GameScene::clearPingLocations() {
	for(list<PingLocation*>::iterator pingLocationsIterator = _pingLocations.begin();
		pingLocationsIterator != _pingLocations.end();
		_pingLocations.erase(pingLocationsIterator++)) {
		this->removeChild(*pingLocationsIterator, true);
		delete *pingLocationsIterator;
	}
}

void GameScene::clearSelectedSparksIfNoAction(float dt) {
	if(!_isManipulatingSparks && !_isManipulatingViewport) {
		_selectedSparks.clear();
		clearPingLocations();
		updateSparkSelectionEffects();
	}
}

void GameScene::touchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
		
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
		
		this->scheduleOnce(schedule_selector(GameScene::clearSelectedSparksIfNoAction), Config::getIntForKey(TOUCH_MOVE_BEGAN_DELAY_MILLIS)/1000.0+0.150);
	}
	
	_lastTouchBeganMillis = now;
}


void GameScene::touchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
	
	if(touches->count() != 1) { return; };
	
	CCTouch* touch = (CCTouch*)touches->anyObject();
	CCPoint location = touch->getLocation();
	CCPoint mapLocation = ccpSub(location, _gameLayer->convertToWorldSpace(CCPointZero));

				
	if(!_isManipulatingSparks && !_isManipulatingViewport) {
		//well by golly, what ARE we doing?
		
		this->unschedule(schedule_selector(GameScene::clearSelectedSparksIfNoAction));
		
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

void GameScene::touchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {

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

			//schedule MOVE command for the next frame
			list<int> ids = Spark::getIdList(_selectedSparks);
			Command* command = new Command(MOVE, _currentFrame+1, SPARK, ids, _currentTouches);
			_commandQueue.push_back(command);
			
			//TODO: send command to network layer to send to oppoonent
				
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
	for(list<Spark*>::iterator sparksIterator = _sparks.begin();
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

void GameScene::deselectSpark(Spark* spark) {
	for(set<Spark*>::iterator selectedSparksIterator = _selectedSparks.begin();
		selectedSparksIterator != _selectedSparks.end();
		selectedSparksIterator++) {

		if((*selectedSparksIterator)->getId() == spark->getId()) {
			_selectedSparks.erase(selectedSparksIterator);
			break;
		}
	}
}




















void GameScene::onMessage(const Json::Value& message) {
	CCLOG("GameScene received message: %s", message.toStyledString().c_str());
	
		
	Json::Value status = message["status"];
	if(status != NULL) {
	
		if(status.asString() == "ok") {


		}else {
			//TODO: handle errors
			CCLOG("Received messge is an error - ignoring");
			
		}
	}else {
		CCLOG("Recevied message has no 'status' element - ignoring");
	}
}

void GameScene::onConnect() {
	CCLOG("Connected to Peer");
			
}

void GameScene::onDisconnect() {
	CCLOG("Disconnected from Peer");		

	//TODO: notify user of lost connection
	
}


void GameScene::onDisconnectChild() {
	CCLOG("Peer disconnected");		

	//TODO: notify user of lost connection
	
}










void GameScene::cleanup() {
			
	for(list<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		_sparks.erase(sparksIterator++)) {
		delete (*sparksIterator);
	}

	for(list<Orb*>::iterator orbsIterator = _orbs.begin();
		orbsIterator != _orbs.end();
		_orbs.erase(orbsIterator++)) {
		delete (*orbsIterator);
	}
}


GameScene::~GameScene() {
	
	cleanup();
	
	_selectedSparks.clear();
	
	for(list<GameStateSnapshot*>::iterator gameStateSnapshotsIterator = _gameStateSnapshots.begin();
		gameStateSnapshotsIterator != _gameStateSnapshots.end();
		_gameStateSnapshots.erase(gameStateSnapshotsIterator++)) {
		delete (*gameStateSnapshotsIterator);
	}

	for(list<Command*>::iterator commandQueueIterator = _commandQueue.begin();
		commandQueueIterator != _commandQueue.end();
		_commandQueue.erase(commandQueueIterator++)) {
		delete (*commandQueueIterator);
	}	


	if(_player != NULL) {
		delete _player;
		_player = NULL;
	}
	if(_opponent != NULL) {
		delete _opponent;
		_opponent = NULL;
	}
}
