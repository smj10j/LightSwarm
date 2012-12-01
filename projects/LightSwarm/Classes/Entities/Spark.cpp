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


void Spark::addSelectionEffect() {
	
	//_sprite->setBlendFunc({GL_BLEND_SRC_ALPHA, GL_BLEND_DST_ALPHA});
	_sprite->runAction(CCTintTo::create(0.25, 100, 100, 255));
}


bool Spark::isInShape(list<CCPoint> shape) {

	float scaleX =_sprite->getScaleX();
	float scaleY = _sprite->getScaleY();

    // This is more accurate point for the node
    CCPoint absPoint = _sprite->convertToWorldSpace(CCPointZero);
	absPoint = ccpAdd(absPoint, ccp(_sprite->getContentSize().width/2 * scaleX,
									_sprite->getContentSize().height/2 * scaleY)
				);
	
	return Utilities::isPointInShape(absPoint, shape);
}

void Spark::setTargetMovePath(list<CCPoint> path, CCPoint viewportCenter) {

	_targetViewportCenter = viewportCenter;

	//clear the queue
	queue<CCPoint> empty;
	swap(_targetMovePath, empty);
	
	//convert path to a queue (only add some of the path items)
	int i = 0;
	
	for(list<CCPoint>::iterator pathIterator = path.begin();
		pathIterator != path.end();
		pathIterator++) {
		
		if(i++ % PATH_SAMPLE_RATE == 0) {
			_targetMovePath.push(ccpSub(*pathIterator, _targetViewportCenter));
		}
	}
	_targetMovePath.push(path.back());
	
	//set our final destination
	_restingPosition = ccpSub(_targetMovePath.back(), _targetViewportCenter);
}

void Spark::update(float dt) {
	if(!_targetMovePath.empty()) {
		//on the move!
		CCPoint pos = _sprite->getPosition();
		CCPoint targetMoveLocation = _targetMovePath.front();
		bool isAtTarget = Utilities::isNear(targetMoveLocation, pos, DIRECT_TOUCH_DISTANCE);
		if(!isAtTarget) {
			float ds = SPARK_SPEED*dt;
			CCPoint v = ccpNormalize(ccp(targetMoveLocation.x-pos.x, targetMoveLocation.y-pos.y));
			
			CCPoint newLocation = ccpAdd(pos, ccp(ds*v.x,ds*v.y));
			newLocation = this->jitter(newLocation, ccpMult(ccpPerp(v), 2), dt);
			//CCLog("Moving sprite to: %f,%f from %f,%f -- target = %f,%f, ds=%f, v=%f,%f", newLocation.x, newLocation.y, pos.x,pos.y, targetMoveLocation.x,targetMoveLocation.y, ds, v.x, v.y);
			
			_sprite->setPosition(newLocation);
		}else {
			_targetMovePath.pop();
		}
	}else {
		//resting
		bool isAtRest = Utilities::isNear(_restingPosition, _sprite->getPosition(), NEARBY_DISTANCE);
		if(isAtRest) {
			//jitter
			CCPoint newLocation = this->jitter(_sprite->getPosition(), ccp(0.5,0.5), dt);
			_sprite->setPosition(newLocation);
						
		}else {
			//move back into scope
			_targetMovePath.push(_restingPosition);
		}
	}
}

CCPoint Spark::jitter(CCPoint point, CCPoint weights, float dt) {
	float ds = SPARK_SPEED*2*dt;
	return ccpAdd(point, ccp((Utilities::getRandomDouble()-0.5)*weights.x*ds, (Utilities::getRandomDouble()-0.5)*weights.y*ds));
}


Spark::~Spark() {
	_sprite->release();
}