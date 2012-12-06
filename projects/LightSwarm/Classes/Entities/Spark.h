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

	Spark(CCNode* parent, CCPoint position, float scaleMultiplier, float speedMultiplier, float strengthMultiplier, float healthMultiplier):
		_parent(parent),
		_position(position),
		_scaleMultiplier(scaleMultiplier),
		_speedMultiplier(speedMultiplier),
		_strengthMultiplier(strengthMultiplier),
		_initialHealth(healthMultiplier*Config::getDoubleForKey(SPARK_BASE_HEALTH)),
		_isDead(false),
		_restingPosition(position) {
		
		_parent->retain();
		
		_nearestOrb = NULL;
		_sprite = NULL;
		_isSelected = false;
		_health = _initialHealth;
		_lifetimeMillis = 0;
		_lastNearestOrbUpdateMillis = -10000;
		_lastCenterUpdateMillis = -10000;
		_lastAtRestJitterMillis = -10000;
		
		_isModifyingState = false;
		_isOnParent = false;

		//this staggers updates - which creates a smoother framerate
		_updateOffset = Utilities::getRandomDouble()*100;
		
		updateCenter();
	}
	
	//copy constructor
	Spark(Spark* spark) {
		
		_parent = spark->_parent;
		_parent->retain();

		_nearestOrb = NULL;
		_sprite = NULL;
		_isModifyingState = false;
		_isOnParent = false;
		
		_isSelected = spark->_isSelected;
		_targetMovePath = spark->_targetMovePath;
		_restingPosition = spark->_restingPosition;
		_position = (spark->_sprite != NULL && spark->_isOnParent) ? spark->_sprite->getPosition() : spark->_position;
		_center = spark->_center;
		_scaleMultiplier = spark->_scaleMultiplier;
		_speedMultiplier = spark->_speedMultiplier;
		_strengthMultiplier = spark->_strengthMultiplier;

		_initialHealth = spark->_initialHealth;
		_health = spark->_health;
		_isDead = spark->_isDead;

		_lifetimeMillis = spark->_lifetimeMillis;
		_updateOffset = spark->_updateOffset;

		_lastNearestOrbUpdateMillis = spark->_lastNearestOrbUpdateMillis;
		_lastCenterUpdateMillis = spark->_lastCenterUpdateMillis;
		_lastAtRestJitterMillis = spark->_lastAtRestJitterMillis;
				
		//CCLOG("Called sprite copy constructor");
	};
	
	void loadSprite();
	void addSpriteToParent();
	void removeSpriteFromParent();

	CCPoint getPosition();
	
	void update(const float dt);
	
	void setTargetMovePath(const list<CCPoint>& path);
	void setViewportScale(const float scale);
	
	void clearAllEffects();
	void addSelectionEffect();

	bool isDead();
	bool isSelected();
	
	bool isInShape(const list<CCPoint>& shape);
	bool isNear(const CCPoint& point);
	bool isNear(const CCPoint& point, const int threshold);

	void setNearestOrb(set<Orb*>& orbs);
	
	static list<CCPoint> getPositionList(const set<Spark*> sparks);
	
	virtual ~Spark();

private:

	CCNode* _parent;
	CCSprite* _sprite;
	
	queue<CCPoint> _targetMovePath;
	CCPoint _restingPosition;
	CCPoint _center;	//absolute coords
	CCPoint _position;	//relative coords
	float _scaleMultiplier;
	
	Orb* _nearestOrb;
	
	float _speedMultiplier;
	float _strengthMultiplier;
	float _health;
	float _initialHealth;
	
	bool _isDead;
	bool _isSelected;//also added to _selectedSparks
	
	double _lifetimeMillis;
	float _updateOffset;
	double _lastNearestOrbUpdateMillis;
	double _lastCenterUpdateMillis;
	double _lastAtRestJitterMillis;
	
	bool _isModifyingState;
	bool _isOnParent;
	
	CCPoint jitter(const CCPoint& point, const CCPoint weights, const float dt);
	void updateCenter();
	void die();
};






#endif /* defined(__LightSwarm__Spark__) */
