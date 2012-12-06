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

	Orb() {
		_sprite = NULL;
		_isModifyingState = false;
		_isOnParent = false;
	}

	Orb(CCNode* parent, CCPoint position, float scaleMultiplier):
		_parent(parent),
		_position(position),
		_scaleMultiplier(scaleMultiplier) {

		_id = 10000*Utilities::getRandomDouble() + 10000*Utilities::getRandomDouble();

		_parent->retain();

		_lifetimeMillis = 0;
		_lastCenterUpdateMillis = -10000;

		_sprite = NULL;
		_isModifyingState = false;
		_isOnParent = false;
					
		_updateOffset = Utilities::getRandomDouble()*100;
		
		updateCenterAndRadius();
	}

	//MY copy constructor - this will grab a big chunk off the heap and allocate it manually
	static void copy(Orb* dst, Orb* src) {
		
		dst->_id = src->_id;
		
		dst->_parent = src->_parent;
		dst->_parent->retain();
		
		dst->_position = (src->_sprite != NULL && src->_isOnParent) ? src->_sprite->getPosition() : src->_position;
		dst->_center = src->_center;
		dst->_radius = src->_radius;
		dst->_scaleMultiplier = src->_scaleMultiplier;

		dst->_lifetimeMillis = src->_lifetimeMillis;
		dst->_updateOffset = src->_updateOffset;

		dst->_lastCenterUpdateMillis = src->_lastCenterUpdateMillis;
	}

	CCPoint getPosition();
	
	void loadSprite();
	void addSpriteToParent();
	void removeSpriteFromParent();
		
	void update(const float dt);
	
	void clearAllEffects();
	void addSelectionEffect();
	
	float getRadius();

	bool isInShape(const list<CCPoint>& shape);
	
	int getId() {
		return _id;
	}
			
	virtual ~Orb();

private:

	CCNode* _parent;
	CCSprite* _sprite;
	int _id;
	
	CCPoint _center;
	CCPoint _position;
	float _scaleMultiplier;
	float _radius;
	
	double _lifetimeMillis;
	double _lastCenterUpdateMillis;
	float _updateOffset;
	
	bool _isModifyingState;
	bool _isOnParent;
	
	void updateCenterAndRadius();
	
};


#endif /* defined(__LightSwarm__Orb__) */
