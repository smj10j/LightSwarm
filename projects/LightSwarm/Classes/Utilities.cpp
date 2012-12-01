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

void Utilities::init() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	NEARBY_DISTANCE = MAX(winSize.width, winSize.height)/10.0;
	IMMEDIATE_VINCINITY_DISTANCE = MAX(winSize.width, winSize.height)/15.0;
	DIRECT_TOUCH_DISTANCE = MAX(20, MAX(winSize.width, winSize.height)/50.0);
}

double Utilities::getMillis() {
	timeval time;
	gettimeofday(&time, NULL);
	return ((double)time.tv_sec) + ((double)time.tv_usec / 1000000.0);
}

bool Utilities::isNear(CCPoint p1, CCPoint p2) {
	return Utilities::isNear(p1, p2, IMMEDIATE_VINCINITY_DISTANCE);
}

bool Utilities::isNear(CCPoint p1, CCPoint p2, int threshold) {
	return fabsf(p1.x-p2.x) < threshold && fabsf(p1.y-p2.y) < threshold;
}

bool Utilities::isPointInShape(CCPoint point, list<CCPoint> shape) {

	//special case
	if(shape.size() == 1) {
		return Utilities::isNear(point, shape.front(), DIRECT_TOUCH_DISTANCE);
	}

	float x = point.x;
    float y = point.y;

    CCPoint prev;
    CCPoint current;
    bool isIn = false;

	list<CCPoint>::iterator prevShapeIterator = --shape.end();
	for(list<CCPoint>::iterator shapeIterator = shape.begin();
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




void Utilities::setRandomSeed(int seed) {
	_randDouble.seed(seed);
}

double Utilities::getRandomDouble() {
	return _randDouble();
}





