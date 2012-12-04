//
//  PingLocation.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/4/12.
//
//

#include "PingLocation.h"

void PingLocation::update(float dt) {
	if(_radius < _targetRadius) {
		_radius+= _targetRadius*dt;		
		_opacity = fmin(_radius/_targetRadius, 1);
	}else {
		if(_opacity > 0) {
			_opacity-= dt/5.0;
		}
	}
}

void PingLocation::draw() {

	if(_opacity <= 0) return;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2);
	ccDrawColor4B(_color.r, _color.g, _color.b, _opacity*255);
	ccDrawCircle(_position, _radius, 0, 60, false);
}