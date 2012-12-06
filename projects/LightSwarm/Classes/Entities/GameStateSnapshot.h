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
		for(set<Spark*>::iterator sparksIterator = gameScene->_sparks.begin();
			sparksIterator != gameScene->_sparks.end();
			sparksIterator++) {
							
			//TODO: on iPad1 with 100 sparks
			//insertion into set takes about 4ms
			//creation of sparks with no data takes about 24ms
			//filling the sparks with data takes about 5ms
			//total of about 33ms
			//BIG WIN COMES FROM FIGURING OUT HOW TO CREATE ITEMS ON THE HEAP EFFICIENTLY
			
			_sparks.insert(new Spark(*sparksIterator));
		}
		
		for(set<Orb*>::iterator orbsIterator = gameScene->_orbs.begin();
			orbsIterator != gameScene->_orbs.end();
			orbsIterator++) {
		
			_orbs.insert(new Orb(*orbsIterator));
		}
		
		_isRestoring = false;
		
		float size = (sizeof(Spark)*_sparks.size() + sizeof(Orb)*_orbs.size() + sizeof(this))/1024.0;
		CCLOG("Created a game state snapshot in %f of approx size %fkb", Utilities::getMillis() - startTime, size);
	}

	virtual ~GameStateSnapshot();
	
	void restoreTo(GameScene* gameScene);

	int _frame;

	set<Orb*> _orbs;
	set<Spark*> _sparks;
	


private:

	bool _isRestoring;
};

#endif /* defined(__LightSwarm__GameStateSnapshot__) */
