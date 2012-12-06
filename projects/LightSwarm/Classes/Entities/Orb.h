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

	Orb(CCNode* parent, CCPoint position, float scaleMultiplier):
		_parent(parent),
		_position(position),
		_scaleMultiplier(scaleMultiplier) {

		_parent->retain();

		_lifetimeMillis = 0;
		_lastCenterUpdateMillis = -10000;

		_sprite = NULL;
		_isModifyingState = false;
		_isOnParent = false;
					
		_updateOffset = Utilities::getRandomDouble()*100;
		
		loadSprite();

		updateCenterAndRadius();
	}
	
	//copy constructor
	Orb(Orb& orb) {
		
		_parent = orb._parent;
		_parent->retain();

		_sprite = NULL;
		_isModifyingState = false;
		_isOnParent = false;
		
		_position = orb._position;
		_center = orb._center;
		_radius = orb._radius;
		_scaleMultiplier = orb._scaleMultiplier;

		_lifetimeMillis = orb._lifetimeMillis;
		_updateOffset = orb._updateOffset;

		_lastCenterUpdateMillis = orb._lastCenterUpdateMillis;
		
		loadSprite();
	};

	CCPoint getPosition();
	
	void loadSprite();
	void addSpriteToParent();
	void removeSpriteFromParent();
		
	void update(const float dt);
	
	void clearAllEffects();
	void addSelectionEffect();
	
	float getRadius();

	bool isInShape(const list<CCPoint>& shape);
	
	virtual ~Orb();

private:

	CCNode* _parent;
	CCSprite* _sprite;
	
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
