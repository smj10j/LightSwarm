//
//  Utilities.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 11/29/12.
//
//

#include "Utilities.h"

MTRand_closed _randDouble;	//double in closed range [0,1]
int DIRECT_TOUCH_DISTANCE = 0;
int IMMEDIATE_VINCINITY_DISTANCE = 0;
int NEARBY_DISTANCE = 0;
float SCALE_FACTOR = 1;

void Utilities::init() {
	CCSize frameSize = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();

	NEARBY_DISTANCE = MAX(frameSize.width, frameSize.height)/10.0;
	IMMEDIATE_VINCINITY_DISTANCE = MAX(frameSize.width, frameSize.height)/15.0;
	DIRECT_TOUCH_DISTANCE = MAX(20, MAX(frameSize.width, frameSize.height)/50.0);
	
	SCALE_FACTOR = CCDirector::sharedDirector()->getContentScaleFactor();
	CCLOG("Screen size = %fx%f - scale factor = %f", frameSize.width, frameSize.height, SCALE_FACTOR);
}

double Utilities::getMillis() {
	timeval time;
	gettimeofday(&time, NULL);
	return ((double)time.tv_sec*1000.0) + ((double)time.tv_usec / 1000.0);
}

bool Utilities::isNear(const CCPoint& p1, const CCPoint& p2) {
	return Utilities::isNear(p1, p2, IMMEDIATE_VINCINITY_DISTANCE);
}

bool Utilities::isNear(const CCPoint& p1, const CCPoint& p2, const int threshold) {
	return getDistance(p1, p2) <= threshold;
}

bool Utilities::isNear(const CCPoint& point, const list<CCPoint>& points) {
	return Utilities::isNear(point, points, IMMEDIATE_VINCINITY_DISTANCE);
}

bool Utilities::isNear(const CCPoint& point, const list<CCPoint>& points, const int threshold) {
	for(list<CCPoint>::const_iterator pointsIterator = points.begin();
		pointsIterator != points.end();
		pointsIterator++) {
		if(Utilities::isNear(point, *pointsIterator, threshold)) {
			return true;
		}
	}
	return false;
}

float Utilities::getDistance(const CCPoint& p1, const CCPoint& p2) {
	float x = p2.x-p1.x;
	float y = p2.y-p1.y;
	return sqrt(x*x + y*y);
}

bool Utilities::isPointInShape(const CCPoint& point, const list<CCPoint>& shape) {

	//special case
	if(shape.size() == 1) {
		return Utilities::isNear(point, shape.front(), DIRECT_TOUCH_DISTANCE);
	}

	float x = point.x;
    float y = point.y;

    CCPoint prev;
    CCPoint current;
    bool isIn = false;

	list<CCPoint>::const_iterator prevShapeIterator = --shape.end();
	for(list<CCPoint>::const_iterator shapeIterator = shape.begin();
		shapeIterator != shape.end();
		prevShapeIterator = shapeIterator++) {
		
		current = *shapeIterator;
		prev = *prevShapeIterator;

        if( ((current.y > y) != (prev.y > y)) && (x < (prev.x -current.x) * (y - current.y) / (prev.y - current.y) + current.x)) {
            isIn = !isIn;
        }
    }
    return isIn;
}




void Utilities::setRandomSeed(const int seed) {
	_randDouble.seed(seed);
}

double Utilities::getRandomDouble() {
	return _randDouble();
}





