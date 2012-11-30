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
	_sprite->runAction(CCTintTo::create(0.50, 255, 255, 255));
}


void Spark::showSelectionEffect(const ccColor3B& colour, const CCSize& size) {

    CCPoint pos = ccp(_sprite->getContentSize().width / 2, _sprite->getContentSize().height / 2);
	
	//_sprite->setBlendFunc({GL_BLEND_SRC_ALPHA, GL_BLEND_DST_ALPHA});
	_sprite->runAction(CCTintTo::create(0.25, 100, 100, 255));
}


bool Spark::isInShape(list<CCPoint> shape) {

    // This is more accurate point for the node
    CCPoint absPoint = _sprite->convertToWorldSpace(CCPointZero);
	absPoint = ccpAdd(absPoint, ccp(_sprite->getContentSize().width/2, _sprite->getContentSize().height/2));
	
	return Utilities::isPointInShape(absPoint, shape);
}

void Spark::setTargetMovePath(list<CCPoint> path) {

	//clear the queue
	queue<CCPoint> empty;
	swap(_targetMovePath, empty);
	
	//convert path to a queue (only add 5 total items)
	int step = path.size()/5.0;
	int i = 0;
	
	for(list<CCPoint>::iterator pathIterator = path.begin();
		pathIterator != path.end();
		pathIterator++) {
		
		if(i++ % step == 0) {
			_targetMovePath.push(*pathIterator);
		}
	}
	
	//set our final destination
	_restingPosition = _targetMovePath.back();
}

void Spark::update(float dt) {
	if(!_targetMovePath.empty()) {
		//on the move!
		CCPoint pos = _sprite->getPosition();
		CCPoint targetMoveLocation = _targetMovePath.front();
		bool isAtTarget = Utilities::isNear(targetMoveLocation, pos, 10);
		if(!isAtTarget) {
			float ds = SPARK_SPEED*dt;
			CCPoint v = ccpNormalize(ccp(targetMoveLocation.x-pos.x, targetMoveLocation.y-pos.y));
			
			CCPoint newLocation = ccpAdd(pos, ccp(ds*v.x,ds*v.y));
			//CCLog("Moving sprite to: %f,%f from %f,%f -- target = %f,%f, ds=%f, v=%f,%f", newLocation.x, newLocation.y, pos.x,pos.y, targetMoveLocation.x,targetMoveLocation.y, ds, v.x, v.y);
			
			_sprite->setPosition(newLocation);
		}else {
			_targetMovePath.pop();
		}
	}else {
		//resting
		bool isAtRest = Utilities::isNear(_restingPosition, _sprite->getPosition(), 50);
		if(isAtRest) {
			//jitter
			float ds = SPARK_SPEED*2*dt;
			CCPoint v = ccp(Utilities::getRandomDouble()-0.5, Utilities::getRandomDouble()-0.5);
			CCPoint newLocation = ccpAdd(_sprite->getPosition(), ccp(ds*v.x,ds*v.y));
			_sprite->setPosition(newLocation);
						
		}else {
			//move back into scope
			_targetMovePath.push(_restingPosition);
		}
	}
}


Spark::~Spark() {
	_sprite->release();
}