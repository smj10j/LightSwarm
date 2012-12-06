//
//  Orb.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/4/12.
//
//

#ifndef __LightSwarm__Orb__
#define __LightSwarm__Orb__

#include "Common.h"
#include <set>
#include <list>
#include <queue>
USING_NS_CC;
using namespace std;

class Orb
{
public:

	Orb(CCSprite* sprite):
		_sprite(sprite){

		_lifetimeMillis = 0;
		_lastCenterUpdateMillis = -10000;
		
		updateCenterAndRadius();
		
		_updateOffset = Utilities::getRandomDouble()*100;

		_sprite->retain();
	}

	CCPoint getPosition();
	
	void update(const float dt);
	
	void clearAllEffects();
	void addSelectionEffect();
	
	float getRadius();

	bool isInShape(const list<CCPoint>& shape);
	
	virtual ~Orb();

private:

	CCSprite* _sprite;
	
	CCPoint _center;
	float _radius;
	
	double _lifetimeMillis;
	double _lastCenterUpdateMillis;
	float _updateOffset;
	
	void updateCenterAndRadius();
	
};


#endif /* defined(__LightSwarm__Orb__) */
