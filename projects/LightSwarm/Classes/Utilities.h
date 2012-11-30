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
USING_NS_CC;

class Utilities
{
public:

	static double getMillis();

	static bool isNear(CCPoint p1, CCPoint p2);
	static bool isNear(CCPoint p1, CCPoint p2, int threshold);

};


#endif /* defined(__LightSwarm__Utilities__) */
