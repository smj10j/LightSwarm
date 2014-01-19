//
//  PingLocation.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/4/12.
//
//

#ifndef __LightSwarm__PingLocation__
#define __LightSwarm__PingLocation__

#include "Common.h"
USING_NS_CC;
using namespace std;

class PingLocation : public CCNode
{
public:

	PingLocation(CCPoint position, int startRadius, int targetRadius, ccColor4B color):
		_position(position),
		_targetRadius(targetRadius),
		_radius(startRadius) {
		
		_color = color;
		_opacity = 0;
	}
	
	void draw();
	void update(float dt);

private:

	float _radius;
	float _targetRadius;
	CCPoint _position;
	ccColor4B _color;
	float _opacity;
	
};


#endif /* defined(__LightSwarm__PingLocation__) */
