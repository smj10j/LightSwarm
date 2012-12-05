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
		
		updateCenterAndRadius();

		_sprite->retain();
	}

	CCSprite* getSprite();
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
	
	void updateCenterAndRadius();
	
};


#endif /* defined(__LightSwarm__Orb__) */
