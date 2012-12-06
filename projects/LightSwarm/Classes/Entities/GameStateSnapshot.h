//
//  GameStateSnapshot.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/6/12.
//
//

#ifndef __LightSwarm__GameStateSnapshot__
#define __LightSwarm__GameStateSnapshot__


#define SPARK_INITIAL_MEMORY_ALLOCATION_SIZE 1000


#include "Common.h"
#include "GameScene.h"
#include <list>
USING_NS_CC;
using namespace std;

class GameStateSnapshot
{
public:

	GameStateSnapshot(GameScene* gameScene):
		_frame(gameScene->_currentFrame) {
	
		_isRestoring = true;
		//double startTime = Utilities::getMillis();
				
		//copy orbs and sparks
		
		_sparksSize = gameScene->_sparks.size();
		if(_sparksSize > SPARK_INITIAL_MEMORY_ALLOCATION_SIZE) {
			_sparksOverflow = new Spark[_sparksSize-SPARK_INITIAL_MEMORY_ALLOCATION_SIZE];
		}else {
			_sparksOverflow = NULL;
		}

		int i = 0;
		for(list<Spark*>::iterator sparksIterator = gameScene->_sparks.begin();
			sparksIterator != gameScene->_sparks.end();
			sparksIterator++) {
							
			if(i < SPARK_INITIAL_MEMORY_ALLOCATION_SIZE) {
				Spark::copy(&_sparks[i++], *sparksIterator, false);
			}else {
				Spark::copy(&_sparksOverflow[i++ - SPARK_INITIAL_MEMORY_ALLOCATION_SIZE], *sparksIterator, false);
			}
		}
		
		_orbsSize = gameScene->_orbs.size();
		_orbs = new Orb[_orbsSize];

		int j = 0;
		for(list<Orb*>::iterator orbsIterator = gameScene->_orbs.begin();
			orbsIterator != gameScene->_orbs.end();
			orbsIterator++) {
		
			Orb::copy(&_orbs[j++], *orbsIterator, false);
		}
		
		_isRestoring = false;
		
		//float size = (sizeof(Spark)*(_sparksSize > SPARK_INITIAL_MEMORY_ALLOCATION_SIZE ? _sparksSize : SPARK_INITIAL_MEMORY_ALLOCATION_SIZE) + sizeof(Orb)*_orbsSize + sizeof(this))/1024.0;
		//CCLOG("Created a game state snapshot in %f of approx size %fkb", Utilities::getMillis() - startTime, size);
	}

	virtual ~GameStateSnapshot();
	
	void restoreTo(GameScene* gameScene);

	int _frame;

	Spark _sparks[SPARK_INITIAL_MEMORY_ALLOCATION_SIZE];
	Spark* _sparksOverflow;
	int _sparksSize;
	
	Orb* _orbs;
	int _orbsSize;

private:

	bool _isRestoring;
};

#endif /* defined(__LightSwarm__GameStateSnapshot__) */
