//
//  Spark.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 11/30/12.
//
//

#include "Spark.h"


CCSprite* Spark::getSprite() {
	return _sprite;
}


void Spark::clearAllEffects() {
	if(_glowSprite != NULL) {
		_glowSprite->removeFromParentAndCleanup(true);
		_glowSprite->release();
		_glowSprite = NULL;
	}
}


void Spark::addGlowEffect(const ccColor3B& colour, const CCSize& size) {

    CCPoint pos = ccp(_sprite->getContentSize().width / 2, _sprite->getContentSize().height / 2);

    _glowSprite = CCSprite::createWithSpriteFrameName("asteroid.png");

    _glowSprite->setColor(colour);
    _glowSprite->setPosition(pos);
    _glowSprite->setRotation(_sprite->getRotation());

    _ccBlendFunc f = {GL_ONE, GL_ONE};
    _glowSprite->setBlendFunc(f);
	
    _sprite->addChild(_glowSprite, -1);

/*
    // Run some animation which scales a bit the glow
    CCSequence* s1 = CCSequence::create(
                     CCScaleTo::create(0.9f, size.width, size.height),
                     CCScaleTo::create(0.9f, size.width*0.75f, size.height*0.75f),
					 NULL);
    CCRepeatForever* r = CCRepeatForever::create(s1);
    _glowSprite->runAction(r);
*/


	_glowSprite->retain();
}


bool Spark::isInShape(list<CCPoint> shape) {

    // This is more accurate point for the node
    CCPoint absPoint = _sprite->convertToWorldSpace(CCPointZero);
	absPoint = ccpAdd(absPoint, ccp(_sprite->getContentSize().width/2, _sprite->getContentSize().height/2));
	    float x = absPoint.x;
    float y = absPoint.y;

    CCPoint prev;
    CCPoint cp;
    bool isIn = false;

	list<CCPoint>::iterator prevShapeIterator = shape.begin();
	for(list<CCPoint>::iterator shapeIterator = shape.begin();
		shapeIterator != shape.end();
		prevShapeIterator = shapeIterator++) {
		
		cp = *shapeIterator;
		prev = *prevShapeIterator;



        if( ((cp.y > y) != (prev.y > y)) && (x < (prev.x -cp.x) * (y - cp.y) / (prev.y - cp.y) + cp.x)) {

			CCLOG("MATCH with coords %f,%f - %f,%f", cp.x,cp.y,prev.x,prev.y);

            isIn = !isIn;
        }
    }
    return isIn;
}





Spark::~Spark() {
	if(_glowSprite != NULL) {
		_glowSprite->release();
	}
	_sprite->release();
}