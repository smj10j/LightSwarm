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

bool Spark::isNear(const CCPoint& point) {
	return isNear(point, IMMEDIATE_VINCINITY_DISTANCE);
}

bool Spark::isNear(const CCPoint& point, const int threshold) {
	return Utilities::isNear(_center, point, threshold);
}

bool Spark::isInShape(const list<CCPoint>& shape) {
	return Utilities::isPointInShape(_center, shape);
}

CCPoint Spark::getPosition() {
	return _center;
}

void Spark::setTargetMovePath(const list<CCPoint>& path, const CCPoint viewportCenter) {

	_targetViewportCenter = viewportCenter;

	//clear the queue
	queue<CCPoint> empty;
	swap(_targetMovePath, empty);
	
	//convert path to a queue (only add some of the path items)
	int i = 0;
	
	for(list<CCPoint>::const_iterator pathIterator = path.begin();
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

void Spark::update(float dt) {

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
			updateCenter();
		}
		
	}else {
		//resting
		CCPoint position = _sprite->getPosition();
		bool isAtRest = Utilities::isNear(_restingPosition, position, NEARBY_DISTANCE);
		if(isAtRest) {
			//jitter 
			CCPoint newLocation = this->jitter(position, ccp(0.5,0.5), dt);
			_sprite->setPosition(newLocation);
			updateCenter();
						
		}else {
			//move back into scope
			_targetMovePath.push(_restingPosition);
		}
	}
	
	//TODO: optimize this code
	//decrease health if out in space
	float nearestOrbAtmosphereRadius = _nearestOrb->getRadius()*Config::getIntForKey(CONFIG_ORB_ATMOSPHERE_RADIUS_MULTIPLIER);
	
	float distance = Utilities::getDistance(_center, _nearestOrb->getPosition());

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
	
	
	if(!_isDead) {
		if(_health < 0) {
			die();
		}else {
			_lifetimeMillis+= dt*1000.0;
		}
	}
}

void Spark::die() {
	_isDead = true;
	//TODO: play death animation
}

void Spark::setNearestOrb(set<Orb*>& orbs) {
	//TODO: this can be optimized further to only occasionally check if we're near a new orb
	if(_nearestOrb == NULL || !_targetMovePath.empty()) {
		float minDistance = 10000000;
		for(set<Orb*>::iterator orbsIterator = orbs.begin();
			orbsIterator != orbs.end();
			orbsIterator++) {
			float distance = ccpDistance((*orbsIterator)->getPosition(), _center);
			if(distance < minDistance) {
				minDistance = distance;
				_nearestOrb = (*orbsIterator);
			}
		}
	}
}

void Spark::updateCenter() {
	// This is more accurate point for the node
	_center = _sprite->convertToWorldSpace(CCPointZero);
	_center = ccpAdd(_center, ccp(_sprite->getContentSize().width/2 * _sprite->getScaleX(),
								  _sprite->getContentSize().height/2 * _sprite->getScaleY())
				);
}

CCPoint Spark::jitter(const CCPoint& point, const CCPoint weights, const float dt) {
	float ds = Config::getDoubleForKey(CONFIG_SPARK_BASE_SPEED)*_speedMultiplier*2*dt;
	return ccpAdd(point, ccp((Utilities::getRandomDouble()-0.5)*weights.x*ds, (Utilities::getRandomDouble()-0.5)*weights.y*ds));
}

list<CCPoint> Spark::getPositionList(const set<Spark*> sparks) {
	list<CCPoint> points;
	for(set<Spark*>::const_iterator sparksIterator = sparks.begin();
		sparksIterator != sparks.end();
		sparksIterator++) {
		points.push_back((*sparksIterator)->getPosition());
	}
	return points;
}



Spark::~Spark() {
	if(_sprite != NULL) {
		_sprite->release();
		_sprite = NULL;
	}
}