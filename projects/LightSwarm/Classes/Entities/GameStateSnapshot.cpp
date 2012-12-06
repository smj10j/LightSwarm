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
	
	gameScene->cleanup();

	gameScene->_fixedTimestepAccumulator = _fixedTimestepAccumulator;
	gameScene->_currentRunningTime = _currentRunningTime;
	Utilities::setRandomSeed(_currentRunningTime);//sync random generators

	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		sparksIterator++) {
		
		Spark* spark = new Spark(**sparksIterator);
		spark->addSpriteToParent();
		
		gameScene->_sparks.insert(spark);
	}
	
	for(set<Orb*>::iterator orbsIterator = _orbs.begin();
		orbsIterator != _orbs.end();
		orbsIterator++) {
		
		Orb* orb = new Orb(**orbsIterator);
		orb->addSpriteToParent();

		gameScene->_orbs.insert(orb);
	}
	
	gameScene->release();
	
	CCLOG("Restored snapshot in %fms", Utilities::getMillis()-startTime);
	_isRestoring = false;
}