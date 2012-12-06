#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "Common.h"
#include "Spark.h"
#include "Orb.h"
#include "PingLocation.h"
#include <set>
#include <list>
USING_NS_CC;
using namespace std;

class GameScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);	
	
	//touch handlers
	virtual void ccTouchesMoved(CCSet* touches, CCEvent* event);
	virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
		
	void clearPingLocations();
		
	virtual ~GameScene();

private:
	CCSpriteBatchNode* _batchNode;
	
	double _currentRunningTime;
	double _fixedTimestepAccumulator;
	bool _isRollingBack;
		
	bool _isManipulatingViewport;
	bool _isManipulatingSparks;
	CCPoint _prevViewporCenter;
	CCPoint _viewportDragVelocity;
	double _lastTouchBeganMillis;
	double _lastTouchEndedMillis;
	
	list<CCPoint> _prevTouches;
	list<CCPoint> _currentTouches;
	
	set<Orb*> _orbs;
	
	set<Spark*> _sparks;
	set<Spark*> _selectedSparks;
	
	
	list<PingLocation*> _pingLocations;
	
	CCLayer* _gameLayer;
	
	void updateSparkSelectionEffects();
	void clearSelectedSparksIfNoAction();
	
	
	void draw();
	void update(float dt);
	void singleUpdateStep(float dt);
};

#endif // __GAME_SCENE_H__
