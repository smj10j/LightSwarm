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
#include <list>
#include <queue>
USING_NS_CC;
using namespace std;

class Spark
{
public:

	//Sparks should ONLY be created with this constructor when they will be filled
	//by using Spark::copy 
	Spark() {
		_nearestOrb = NULL;
		_parent = NULL;
		_sprite = NULL;
		_isModifyingState = false;
		_isOnParent = false;
		_isSelected = false;
	}

	Spark(CCNode* parent, CCPoint position, float scaleMultiplier, float speedMultiplier, float strengthMultiplier, float healthMultiplier):
		_parent(parent),
		_position(position),
		_scaleMultiplier(scaleMultiplier),
		_speedMultiplier(speedMultiplier),
		_strengthMultiplier(strengthMultiplier),
		_initialHealth(healthMultiplier*Config::getDoubleForKey(SPARK_BASE_HEALTH)),
		_isDead(false),
		_restingPosition(position) {
		
		static int ID_COUNTER = 0;
		_id = ID_COUNTER++;
		
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
	}
	
	//MY copy constructor - this will grab a big chunk off the heap and allocate it manually
	static void copy(Spark* dst, Spark* src, bool reuseDstSprite) {
		
		dst->_id = src->_id;
		
		dst->_targetMovePath = src->_targetMovePath;
		dst->_restingPosition = src->_restingPosition;
		dst->_position = (src->_sprite != NULL && src->_isOnParent) ? src->_sprite->getPosition() : src->_position;
		dst->_center = src->_center;
		dst->_scaleMultiplier = src->_scaleMultiplier;
		dst->_speedMultiplier = src->_speedMultiplier;
		dst->_strengthMultiplier = src->_strengthMultiplier;

		dst->_initialHealth = src->_initialHealth;
		dst->_health = src->_health;
		dst->_isDead = src->_isDead;

		dst->_lifetimeMillis = src->_lifetimeMillis;
		dst->_updateOffset = src->_updateOffset;

		dst->_lastNearestOrbUpdateMillis = src->_lastNearestOrbUpdateMillis;
		dst->_lastCenterUpdateMillis = src->_lastCenterUpdateMillis;
		dst->_lastAtRestJitterMillis = src->_lastAtRestJitterMillis;

		if(reuseDstSprite && dst->_sprite != NULL) {
			dst->_sprite->setPosition(dst->_position);
			dst->_sprite->setScale(SCALE_FACTOR*dst->_scaleMultiplier);
			
			dst->updateCenter();
			
		}else {
			if(dst->_sprite != NULL) {
				dst->removeSpriteFromParent();
				dst->_sprite->release();
				dst->_sprite = NULL;
			}
			if(dst->_parent != NULL) {
				dst->_parent->release();
				dst->_parent = NULL;
			}
			
			if(src->_parent != NULL) {
				dst->_parent = src->_parent;
				dst->_parent->retain();
			}					
		}
				
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

	void setNearestOrb(const list<Orb*>& orbs);
	
	int getId() {
		return _id;
	}	
	
	static list<CCPoint> getPositionList(const set<Spark*> sparks);
	static list<int> getIdList(const set<Spark*> sparks);
	
	virtual ~Spark();

private:

	CCNode* _parent;
	CCSprite* _sprite;
	int _id;
	
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
