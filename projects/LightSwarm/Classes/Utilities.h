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

class Utilities
{
public:

	static double getMillis();

	static bool isNear(CCPoint p1, CCPoint p2);
	static bool isNear(CCPoint p1, CCPoint p2, int threshold);

	static bool isPointInShape(CCPoint point, list<CCPoint> shape);


	static void setRandomSeed(int seed);
	static double getRandomDouble();
};


#endif /* defined(__LightSwarm__Utilities__) */
