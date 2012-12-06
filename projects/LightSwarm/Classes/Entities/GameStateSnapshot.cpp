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

	gameScene->_currentFrame = _frame;
	Utilities::setRandomSeed(_frame);//sync random generators

	for(int i = 0; i < _sparksSize; i++) {
		
		Spark* spark = new Spark(_sparks[i]);
		spark->addSpriteToParent();
				
		gameScene->_sparks.insert(spark);
	}
	
	for(int i = 0; i < _orbsSize; i++) {
		
		Orb* orb = new Orb(_orbs[i]);
		orb->addSpriteToParent();

		gameScene->_orbs.insert(orb);
	}
	
	gameScene->release();
	
	_isRestoring = false;

	CCLOG("Restored snapshot in %fms", Utilities::getMillis()-startTime);
}

GameStateSnapshot::~GameStateSnapshot() {
	if(_sparks != NULL) {
		delete[] _sparks;
		_sparks = NULL;
	}

	if(_orbs != NULL) {
		delete[] _orbs;
		_orbs = NULL;
	}
}