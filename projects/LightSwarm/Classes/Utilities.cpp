//
//  Utilities.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 11/29/12.
//
//

#include "Utilities.h"

double Utilities::getMillis() {
	timeval time;
	gettimeofday(&time, NULL);
	return ((double)time.tv_sec) + ((double)time.tv_usec / 1000000.0);
}

bool Utilities::isNear(CCPoint p1, CCPoint p2) {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	return Utilities::isNear(p1, p2, MAX(winSize.width, winSize.height)/10.0);
}

bool Utilities::isNear(CCPoint p1, CCPoint p2, int threshold) {
	return fabsf(p1.x-p2.x) < threshold && fabsf(p1.y-p2.y) < threshold;
}