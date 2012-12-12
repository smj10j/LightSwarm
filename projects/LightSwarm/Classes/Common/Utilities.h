//
//  Utilities.h
//  LightSwarm
//
//  Created by Stephen Johnson on 11/29/12.
//
//

#ifndef __LightSwarm__Utilities__
#define __LightSwarm__Utilities__

#include "Common.h"
#include "mtrand.h"
#include <list>
USING_NS_CC;
using namespace std;

extern int DIRECT_TOUCH_DISTANCE;
extern int IMMEDIATE_VINCINITY_DISTANCE;
extern int NEARBY_DISTANCE;

extern float SCALE_FACTOR;

class Utilities
{
public:

	static void init();

	static double getMillis();

	static bool isNear(const CCPoint& p1, const CCPoint& p2);
	static bool isNear(const CCPoint& p1, const CCPoint& p2, const int threshold);

	static bool isNear(const CCPoint& point, const list<CCPoint>& points);
	static bool isNear(const CCPoint& point, const list<CCPoint>& points, const int threshold);

	static bool isPointInShape(const CCPoint& point, const list<CCPoint>& shape);

	static float getDistance(const CCPoint& p1, const CCPoint& p2);

	static void setRandomSeed(const int seed);
	static double getRandomDouble();
	
	
	static string HMAC_SHA1(const string& key, const string& secret);
};


#endif /* defined(__LightSwarm__Utilities__) */
