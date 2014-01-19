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
	Size frameSize = CCDirector::getInstance()->getOpenGLView()->getFrameSize();

	NEARBY_DISTANCE = MAX(frameSize.width, frameSize.height)/10.0;
	IMMEDIATE_VINCINITY_DISTANCE = MAX(frameSize.width, frameSize.height)/15.0;
	DIRECT_TOUCH_DISTANCE = MAX(20, MAX(frameSize.width, frameSize.height)/50.0);
	
	SCALE_FACTOR = CCDirector::getInstance()->getContentScaleFactor();
	CCLOG("Screen size = %fx%f - scale factor = %f", frameSize.width, frameSize.height, SCALE_FACTOR);
}

double Utilities::getMillis() {
	timeval time;
	gettimeofday(&time, NULL);
	return ((double)time.tv_sec*1000.0) + ((double)time.tv_usec / 1000.0);
}

bool Utilities::isNear(const Point& p1, const Point& p2) {
	return Utilities::isNear(p1, p2, IMMEDIATE_VINCINITY_DISTANCE);
}

bool Utilities::isNear(const Point& p1, const Point& p2, const int threshold) {
	float x = p2.x-p1.x;
	float y = p2.y-p1.y;
	return (x*x + y*y) <= (threshold*threshold);
}

bool Utilities::isNear(const Point& point, const list<Point>& points) {
	return Utilities::isNear(point, points, IMMEDIATE_VINCINITY_DISTANCE);
}

bool Utilities::isNear(const Point& point, const list<Point>& points, const int threshold) {
	for(list<Point>::const_iterator pointsIterator = points.begin();
		pointsIterator != points.end();
		pointsIterator++) {
		if(Utilities::isNear(point, *pointsIterator, threshold)) {
			return true;
		}
	}
	return false;
}

float Utilities::getDistance(const Point& p1, const Point& p2) {
	float x = p2.x-p1.x;
	float y = p2.y-p1.y;
	return sqrt(x*x + y*y);
}

bool Utilities::isPointInShape(const Point& point, const list<Point>& shape) {

	//special cases
	if(shape.empty()) {
		return false;
	}
	if(shape.size() == 1) {
		return Utilities::isNear(point, shape.front(), DIRECT_TOUCH_DISTANCE);
	}

	float x = point.x;
    float y = point.y;

    Point prev;
    Point current;
    bool isIn = false;

	list<Point>::const_iterator prevShapeIterator = --shape.end();
	for(list<Point>::const_iterator shapeIterator = shape.begin();
		shapeIterator != shape.end();
		prevShapeIterator = shapeIterator++) {
		
		current = *shapeIterator;
		prev = *prevShapeIterator;

		bool crossesVertically = ((current.y > y) != (prev.y > y));
				
        if( crossesVertically &&
			(x < (prev.x - current.x) * (y - current.y) / (prev.y - current.y) + current.x)) {
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




string Utilities::HMAC_SHA1(const string& key, const string& secret) {

	//TODO: IMPLEMENT

	return "";
}



