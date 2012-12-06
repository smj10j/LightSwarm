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
		_initialHealth(healthMultiplier*Config::getDoubleForKey(SPARK_BASE_HEALTH)),
		_isDead(false),
		_restingPosition(sprite->getPosition()) {
		
		_nearestOrb = NULL;
		_health = _initialHealth;
		_lifetimeMillis = 0;
		_lastNearestOrbUpdateMillis = -10000;
		_lastCenterUpdateMillis = -10000;
		_lastAtRestJitterMillis = -10000;

		//this staggers updates - which creates a smoother framerate
		_updateOffset = Utilities::getRandomDouble()*100;

		updateCenter();
		
		_sprite->retain();
	}

	CCPoint getPosition();
	
	void update(const float dt);
	
	void setTargetMovePath(const list<CCPoint>& path);
	void setViewportScale(const float scale);
	
	void clearAllEffects();
	void addSelectionEffect();

	void remove();
	bool isDead();
	
	bool isInShape(const list<CCPoint>& shape);
	bool isNear(const CCPoint& point);
	bool isNear(const CCPoint& point, const int threshold);

	void setNearestOrb(set<Orb*>& orbs);
	
	static list<CCPoint> getPositionList(const set<Spark*> sparks);
	
	virtual ~Spark();

private:

	CCSprite* _sprite;
	
	queue<CCPoint> _targetMovePath;
	CCPoint _restingPosition;
	CCPoint _center;
	
	Orb* _nearestOrb;
	
	float _speedMultiplier;
	float _strengthMultiplier;
	float _health;
	float _initialHealth;
	
	bool _isDead;
	
	double _lifetimeMillis;
	float _updateOffset;
	double _lastNearestOrbUpdateMillis;
	double _lastCenterUpdateMillis;
	double _lastAtRestJitterMillis;
	
	CCPoint jitter(const CCPoint& point, const CCPoint weights, const float dt);
	void updateCenter();
	void die();
};






#endif /* defined(__LightSwarm__Spark__) */
