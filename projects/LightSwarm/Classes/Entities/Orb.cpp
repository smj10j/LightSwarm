//
//  Orb.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/4/12.
//
//

#include "Orb.h"



CCSprite* Orb::getSprite() {
	return _sprite;
}


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
	double now = Utilities::getMillis();
	if(now - _lastCenterUpdateMillis < 1000) return;
	_lastCenterUpdateMillis = now;


	// This is more accurate point for the node
	_center = _sprite->convertToWorldSpace(CCPointZero);
	_center = ccpAdd(_center, ccp(_sprite->getContentSize().width/2 * _sprite->getScaleX(),
								  _sprite->getContentSize().height/2 * _sprite->getScaleY())
				);
				
	_radius = _sprite->getScale()*max(_sprite->getContentSize().width, _sprite->getContentSize().height);
}

bool Orb::isInShape(const list<CCPoint>& shape) {
	return Utilities::isPointInShape(_center, shape);
}


void Orb::update(const float dt) {

	updateCenterAndRadius();
}



Orb::~Orb() {
	_sprite->release();
}