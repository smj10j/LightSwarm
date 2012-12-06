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
	gameScene->_currentRunningTimeMills = _currentRunningTimeMills;
	gameScene->_randomGeneratorSeed = _randomGeneratorSeed;
	Utilities::setRandomSeed(_randomGeneratorSeed);//sync random generators

	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		sparksIterator++) {
		
		Spark* spark = new Spark(**sparksIterator);
		spark->addSpriteToParent();
		
		if(spark->isSelected()) {
			gameScene->_selectedSparks.insert(spark);
		}
		
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
	
	_isRestoring = false;

	CCLOG("Restored snapshot in %fms", Utilities::getMillis()-startTime);
}

GameStateSnapshot::~GameStateSnapshot() {
	for(set<Spark*>::iterator sparksIterator = _sparks.begin();
		sparksIterator != _sparks.end();
		sparksIterator++) {
		Spark* spark = *sparksIterator;
		delete spark;
	}
	_sparks.clear();

	for(set<Orb*>::iterator orbsIterator = _orbs.begin();
		orbsIterator != _orbs.end();
		orbsIterator++) {
		Orb* orb = *orbsIterator;
		delete orb;
	}
	_orbs.clear();
}