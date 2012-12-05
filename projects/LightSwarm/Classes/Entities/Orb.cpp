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
	return _sprite->getScale()*max(_sprite->getContentSize().width, _sprite->getContentSize().height);
}

CCPoint Orb::getPosition() {
	return _center;
}

bool Orb::isInShape(list<CCPoint>& shape) {

	float scaleX =_sprite->getScaleX();
	float scaleY = _sprite->getScaleY();

    // This is more accurate point for the node
    CCPoint absPoint = _sprite->convertToWorldSpace(CCPointZero);
	absPoint = ccpAdd(absPoint, ccp(_sprite->getContentSize().width/2 * scaleX,
									_sprite->getContentSize().height/2 * scaleY)
				);
	
	return Utilities::isPointInShape(absPoint, shape);
}


void Orb::update(float dt) {


}



Orb::~Orb() {
	_sprite->release();
}