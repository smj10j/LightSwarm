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
		
		if(i++ % Config::getIntForKey(CONFIG_SPARK_PATH_SAMPLE_RATE) == 0) {
			_targetMovePath.push(ccpSub(*pathIterator, _targetViewportCenter));
		}
	}
	_targetMovePath.push(ccpSub(path.back(), _targetViewportCenter));
	
	//set our final destination
	_restingPosition = _targetMovePath.back();

}

bool Spark::isDead() {
	return _isDead;
}

void Spark::update(Orb* orb, float dt) {

	_nearestOrb = orb;

	//handle movement
	if(!_targetMovePath.empty()) {
		//on the move!
		CCPoint position = _sprite->getPosition();
		CCPoint targetMoveLocation = _targetMovePath.front();
		bool isAtTarget = Utilities::isNear(targetMoveLocation, position, DIRECT_TOUCH_DISTANCE);
		if(!isAtTarget) {
			float ds = Config::getDoubleForKey(CONFIG_SPARK_BASE_SPEED)*_speedMultiplier*dt;
			CCPoint v = ccpNormalize(ccp(targetMoveLocation.x-position.x, targetMoveLocation.y-position.y));
			
			CCPoint newLocation = ccpAdd(position, ccp(ds*v.x,ds*v.y));
			newLocation = this->jitter(newLocation, ccpMult(ccpPerp(v), 2), dt);
			//CCLog("Moving sprite to: %f,%f from %f,%f -- target = %f,%f, ds=%f, v=%f,%f", newLocation.x, newLocation.y, pos.x,pos.y, targetMoveLocation.x,targetMoveLocation.y, ds, v.x, v.y);
			
			_sprite->setPosition(newLocation);
		}else {
			_targetMovePath.pop();
		}
	}else {
		//resting
		CCPoint position = _sprite->getPosition();
		bool isAtRest = Utilities::isNear(_restingPosition, position, NEARBY_DISTANCE);
		if(isAtRest) {
			//jitter
			CCPoint newLocation = this->jitter(position, ccp(0.5,0.5), dt);
			_sprite->setPosition(newLocation);
						
		}else {
			//move back into scope
			_targetMovePath.push(_restingPosition);
		}
	}
	
	
	//decrease health if out in space
	float nearestOrbAtmosphereRadius = _nearestOrb->getRadius()*Config::getIntForKey(CONFIG_ORB_ATMOSPHERE_RADIUS_MULTIPLIER);
	float distance = ccpDistance(_sprite->getPosition(), _nearestOrb->getSprite()->getPosition());

	if(distance > nearestOrbAtmosphereRadius) {
		_health-= Config::getDoubleForKey(CONFIG_SPARK_HEALTH_COST_PER_SECOND_WHEN_TRAVELING)*dt;
		
		_sprite->setOpacity(255.0*(_health/_initialHealth));
		
	}else {
		//heal
		if(_health < _initialHealth) {
			//TODO: healRate will also be modified by hospitals
			float healRate = Config::getDoubleForKey(CONFIG_ORB_BASE_HEAL_RATE_PER_SECOND);
			_health+= healRate*dt;
			_sprite->setOpacity(255.0*(_health/_initialHealth));
			
		}else if(_health > _initialHealth) {
			_health = _initialHealth;
			_sprite->setOpacity(255.0*(_health/_initialHealth));
			
		}
	}
	
	
	
	if(!_isDead && _health < 0) {
		die();
	}
}

void Spark::die() {
	_isDead = true;
	//TODO: play death animation
	
	if(_sprite != NULL) {
		_sprite->removeFromParentAndCleanup(true);
		_sprite->release();
		_sprite = NULL;
	}
}

CCPoint Spark::jitter(CCPoint point, CCPoint weights, float dt) {
	float ds = Config::getDoubleForKey(CONFIG_SPARK_BASE_SPEED)*_speedMultiplier*2*dt;
	return ccpAdd(point, ccp((Utilities::getRandomDouble()-0.5)*weights.x*ds, (Utilities::getRandomDouble()-0.5)*weights.y*ds));
}


Spark::~Spark() {
	if(_sprite != NULL) {
		_sprite->release();
		_sprite = NULL;
	}
}