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
USING_NS_CC;
using namespace std;

class GameStateSnapshot
{
public:

	GameStateSnapshot(GameScene* gameScene):
		_frame(gameScene->_currentFrame) {
	
		_isRestoring = true;
		double startTime = Utilities::getMillis();
				
		//copy orbs and sparks
		
		_sparksSize = gameScene->_sparks.size();
		_sparks = new Spark[_sparksSize];

		int i = 0;
		for(set<Spark*>::iterator sparksIterator = gameScene->_sparks.begin();
			sparksIterator != gameScene->_sparks.end();
			sparksIterator++) {
							
			//TODO: on iPad1 with 1000 sparks
			//fill of all sparks takes 5ms
			//heap allocation is taking 17ms
			//total of about 22s
			//BIG WIN COMES FROM FIGURING OUT HOW TO CREATE ITEMS ON THE HEAP EFFICIENTLY
			
			Spark::copy(&_sparks[i++], *sparksIterator);
			//_sparks.insert(new Spark(*sparksIterator));
		}
		
		_orbsSize = gameScene->_orbs.size();
		_orbs = new Orb[_orbsSize];

		int j = 0;
		for(set<Orb*>::iterator orbsIterator = gameScene->_orbs.begin();
			orbsIterator != gameScene->_orbs.end();
			orbsIterator++) {
		
			Orb::copy(&_orbs[j++], *orbsIterator);
		}
		
		_isRestoring = false;
		
		float size = (sizeof(Spark)*_sparksSize + sizeof(Orb)*_orbsSize + sizeof(this))/1024.0;
		CCLOG("Created a game state snapshot in %f of approx size %fkb", Utilities::getMillis() - startTime, size);
	}

	virtual ~GameStateSnapshot();
	
	void restoreTo(GameScene* gameScene);

	int _frame;

	Spark* _sparks;
	int _sparksSize;
	
	Orb* _orbs;
	int _orbsSize;

private:

	bool _isRestoring;
};

#endif /* defined(__LightSwarm__GameStateSnapshot__) */
