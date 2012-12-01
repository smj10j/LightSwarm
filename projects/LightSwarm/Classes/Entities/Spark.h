//
//  Spark.h
//  LightSwarm
//
//  Created by Stephen Johnson on 11/30/12.
//
//

#ifndef __LightSwarm__Spark__
#define __LightSwarm__Spark__

#include "Common.h"
#include <set>
#include <list>
#include <queue>
USING_NS_CC;
using namespace std;

class Spark
{
public:

	Spark(CCSprite* sprite):_sprite(sprite) {
		_sprite->retain();
		_restingPosition = _sprite->getPosition();
	}

	CCSprite* getSprite();
	
	void update(float dt);
	
	void setTargetMovePath(list<CCPoint> path, CCPoint viewportCenter);
	
	void clearAllEffects();
	void addSelectionEffect();
	
	bool isInShape(list<CCPoint> shape);
	
	~Spark();

private:

	CCSprite* _sprite;
	
	queue<CCPoint> _targetMovePath;
	CCPoint _targetViewportCenter;
	CCPoint _restingPosition;
	
	
	
	CCPoint jitter(CCPoint point, CCPoint weights, float dt);
};


#define PATH_SAMPLE_RATE 10
#define SPARK_SPEED 100		//TODO: need to figure out how to do this all with proper units




#endif /* defined(__LightSwarm__Spark__) */
