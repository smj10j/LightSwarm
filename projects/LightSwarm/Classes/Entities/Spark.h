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
#include "Orb.h"
#include <set>
#include <list>
#include <queue>
USING_NS_CC;
using namespace std;

class Spark
{
public:

	Spark(CCSprite* sprite, float speedMultiplier, float strengthMultiplier, float healthMultiplier):
		_sprite(sprite),
		_speedMultiplier(speedMultiplier),
		_strengthMultiplier(strengthMultiplier),
		_initialHealth(healthMultiplier*Config::getDoubleForKey(CONFIG_SPARK_BASE_HEALTH)),
		_isDead(false),
		_restingPosition(sprite->getPosition()) {
		
		_nearestOrb = NULL;
		_health = _initialHealth;
		
		_sprite->retain();
	}

	CCSprite* getSprite();
	
	void update(Orb* orb, float dt);
	
	void setTargetMovePath(list<CCPoint> path, CCPoint viewportCenter);
	void setViewportScale(float scale);
	
	void clearAllEffects();
	void addSelectionEffect();

	void die();
	
	bool isDead();
	
	bool isInShape(list<CCPoint> shape);
	
	virtual ~Spark();

private:

	CCSprite* _sprite;
	
	queue<CCPoint> _targetMovePath;
	CCPoint _targetViewportCenter;
	CCPoint _restingPosition;
	
	Orb* _nearestOrb;
	
	float _speedMultiplier;
	float _strengthMultiplier;
	float _health;
	float _initialHealth;
	
	bool _isDead;
	
	CCPoint jitter(CCPoint point, CCPoint weights, float dt);
};






#endif /* defined(__LightSwarm__Spark__) */
