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
	
		//TODO: copy orbs and sparks
		for(set<Spark*>::iterator sparksIterator = gameScene->_sparks.begin();
			sparksIterator != gameScene->_sparks.end();
			sparksIterator++) {
						
			Spark* spark = new Spark(**sparksIterator);
						
			_sparks.insert(spark);
		}
		
		for(set<Orb*>::iterator orbsIterator = gameScene->_orbs.begin();
			orbsIterator != gameScene->_orbs.end();
			orbsIterator++) {
			
			Orb* orb = new Orb(**orbsIterator);

			_orbs.insert(orb);
		}		
	}
	

	double _currentRunningTime;
	float _fixedTimestepAccumulator;

	set<Orb*> _orbs;
	set<Spark*> _sparks;

private:


};

#endif /* defined(__LightSwarm__GameStateSnapshot__) */
