#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "Common.h"
#include "mtrand.h"
#include "Spark.h"
#include <set>
#include <list>
USING_NS_CC;
using namespace std;

class HelloWorld : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
	
	void draw();
	void update(float dt);
	
	
	//touch handlers
	virtual void ccTouchesMoved(CCSet* touches, CCEvent* event);
	virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	
	~HelloWorld();

private:
	CCSpriteBatchNode* _batchNode;
	
	MTRand_closed _randDouble;	//double in closed range [0,1]
	
	list<CCPoint> _prevTouches;
	list<CCPoint> _currentTouches;
	set<Spark*> _sparks;
	set<Spark*> _selectedSparks;
};

#endif // __HELLOWORLD_SCENE_H__
