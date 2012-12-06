//
//  GameStateSnapshot.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/6/12.
//
//

#include "GameStateSnapshot.h"



void GameStateSnapshot::restoreTo(GameScene* gameScene) {
	if(_isRestoring) return;
	
	_isRestoring = true;
	double startTime = Utilities::getMillis();
	
	gameScene->retain();
	
	gameScene->_currentFrame = _frame;
	Utilities::setRandomSeed(_frame);//sync random generators

	list<Spark*>::iterator gameSceneSparksIterator = gameScene->_sparks.begin();

	for(int i = 0; i < _sparksSize; i++) {

		Spark* spark = NULL;
		bool reuseDstSprite = false;
				
		if(gameSceneSparksIterator != gameScene->_sparks.end()) {
			//replace existing sparks for memory efficiency
			spark = *gameSceneSparksIterator++;
			reuseDstSprite = true;
			
		}else {
			//create a new entry
			spark = new Spark();
			gameScene->_sparks.push_back(spark);
		}
		
		
		if(i < SPARK_INITIAL_MEMORY_ALLOCATION_SIZE) {
			Spark::copy(spark, &_sparks[i], reuseDstSprite);
		}else {
			Spark::copy(spark, &_sparksOverflow[i-SPARK_INITIAL_MEMORY_ALLOCATION_SIZE], reuseDstSprite);
		}
		
		spark->addSpriteToParent();		
	}
	
	//remove any leftovers
	while(gameSceneSparksIterator != gameScene->_sparks.end()) {
		delete *gameSceneSparksIterator;
		gameScene->_sparks.erase(gameSceneSparksIterator++);
	}
	
	
	for(int i = 0; i < _orbsSize; i++) {
		
		Orb* orb = new Orb();
		bool reuseDstSprite = false;
		
		
		Orb::copy(orb, &_orbs[i], reuseDstSprite);
		
		gameScene->_orbs.push_back(orb);

		orb->addSpriteToParent();
	}
	
	gameScene->release();
	
	_isRestoring = false;

	CCLOG("Restored snapshot in %fms", Utilities::getMillis()-startTime);
}

GameStateSnapshot::~GameStateSnapshot() {
	if(_sparksOverflow != NULL) {
		delete[] _sparksOverflow;
		_sparksOverflow = NULL;
	}

	if(_orbs != NULL) {
		delete[] _orbs;
		_orbs = NULL;
	}
}