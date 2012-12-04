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
		
		_sprite->retain();
	}

	CCSprite* getSprite();
	
	void update(float dt);
	
	void clearAllEffects();
	void addSelectionEffect();
	
	float getRadius();

	bool isInShape(list<CCPoint> shape);
	
	virtual ~Orb();

private:

	CCSprite* _sprite;
	
};


#endif /* defined(__LightSwarm__Orb__) */
