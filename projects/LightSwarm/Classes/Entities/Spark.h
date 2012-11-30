//
//  Spark.h
//  LightSwarm
//
//  Created by Stephen Johnson on 11/30/12.
//
//

#ifndef __LightSwarm__Spark__
#define __LightSwarm__Spark__

#include "Common.h"
#include <set>
#include <list>
#include <queue>
USING_NS_CC;
using namespace std;

class Spark
{
public:

	Spark(CCSprite* sprite):_sprite(sprite) {
		_sprite->retain();
		_glowSprite = NULL;
	}

	CCSprite* getSprite();
	
	void update(float dt);
	
	void setTargetMovePath(list<CCPoint> path);
	
	void clearAllEffects();
	void showSelectionEffect(const ccColor3B& colour, const CCSize& size);
	
	bool isInShape(list<CCPoint> shape);
	
	~Spark();

private:

	CCSprite* _sprite;
	CCSprite* _glowSprite;
	
	queue<CCPoint> _targetMovePath;
};



#endif /* defined(__LightSwarm__Spark__) */
