//
//  Orb.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/4/12.
//
//

#include "Orb.h"


void Orb::clearAllEffects() {
	_sprite->runAction(CCTintTo::create(0.50, 255, 255, 255));
}


void Orb::addSelectionEffect() {
	
	//_sprite->setBlendFunc({GL_BLEND_SRC_ALPHA, GL_BLEND_DST_ALPHA});
	_sprite->runAction(CCTintTo::create(0.25, 100, 100, 255));
}

float Orb::getRadius() {
	return _radius;
}

CCPoint Orb::getPosition() {
	return _center;
}


void Orb::updateCenterAndRadius() {
	if(!_isOnParent) return;
	if(_lifetimeMillis - _lastCenterUpdateMillis > 500+_updateOffset) {
	
		_lastCenterUpdateMillis = _lifetimeMillis;

		// This is more accurate point for the node
		_center = ccpSub(_sprite->convertToWorldSpace(CCPointZero), _sprite->getParent()->convertToWorldSpace(CCPointZero));
		_center = ccpAdd(_center, ccp(_sprite->getContentSize().width/2 * _sprite->getScaleX(),
									  _sprite->getContentSize().height/2 * _sprite->getScaleY())
					);
					
		_radius = _sprite->getScale()*max(_sprite->getContentSize().width, _sprite->getContentSize().height);
	}
}

void Orb::loadSprite() {
	_isModifyingState = true;
	if(_sprite != NULL) {
		removeSpriteFromParent();
		_sprite->release();
		_sprite = NULL;
	}
	
	_sprite = CCSprite::createWithSpriteFrameName("asteroid.png");
	_sprite->setPosition(_position);
	_sprite->setScale(SCALE_FACTOR*_scaleMultiplier);
	
	_sprite->retain();
	_isModifyingState = false;
	
}


void Orb::addSpriteToParent() {
	if(_sprite == NULL) {
		loadSprite();
	}
	if(!_isOnParent) {
		_isModifyingState = true;
		_parent->addChild(_sprite, 10);
		_isOnParent = true;
		_isModifyingState = false;
	}
}

void Orb::removeSpriteFromParent() {
	if(_sprite != NULL) {
		_isModifyingState = true;
		if(_isOnParent) {
			_position = _sprite->getPosition();
			_isOnParent = false;
			_sprite->removeFromParentAndCleanup(true);
		}
		_isModifyingState = false;
	}
}

bool Orb::isInShape(const list<CCPoint>& shape) {
	return Utilities::isPointInShape(_center, shape);
}


void Orb::update(const float dt) {

	updateCenterAndRadius();
	
	_lifetimeMillis+= dt*1000.0;
}


Orb::~Orb() {
	if(_sprite != NULL) {
		removeSpriteFromParent();
		_sprite->release();
		_sprite = NULL;
	}
	if(_parent != NULL) {
		_parent->release();
		_parent = NULL;
	}
}