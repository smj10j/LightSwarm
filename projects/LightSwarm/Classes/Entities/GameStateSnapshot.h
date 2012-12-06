//
//  GameStateSnapshot.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/6/12.
//
//

#ifndef __LightSwarm__GameStateSnapshot__
#define __LightSwarm__GameStateSnapshot__


#include "Common.h"
#include "GameScene.h"
#include <set>
#include <list>
#include <queue>
USING_NS_CC;
using namespace std;

class GameStateSnapshot
{
public:

	GameStateSnapshot(GameScene* gameScene):
		_currentRunningTime(gameScene->_currentRunningTime),
		_fixedTimestepAccumulator(gameScene->_fixedTimestepAccumulator) {
		Utilities::setRandomSeed(_currentRunningTime);//sync random generators
	
		//copy orbs and sparks
		for(set<Spark*>::iterator sparksIterator = gameScene->_sparks.begin();
			sparksIterator != gameScene->_sparks.end();
			sparksIterator++) {
												
			_sparks.insert(new Spark(**sparksIterator));
		}
		
		for(set<Orb*>::iterator orbsIterator = gameScene->_orbs.begin();
			orbsIterator != gameScene->_orbs.end();
			orbsIterator++) {
		
			_orbs.insert(new Orb(**orbsIterator));
		}
		
		CCLOG("Created a game state snapshot");
	}
	
	void restoreTo(GameScene* gameScene);

	double _currentRunningTime;
	float _fixedTimestepAccumulator;

	set<Orb*> _orbs;
	set<Spark*> _sparks;

private:

	bool _isRestoring;
};

#endif /* defined(__LightSwarm__GameStateSnapshot__) */
