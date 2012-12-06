#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "Common.h"
#include "Spark.h"
#include "Orb.h"
#include "PingLocation.h"
#include "Command.h"
#include <set>
#include <list>
USING_NS_CC;
using namespace std;

//forward declaration and GameStateSnapshot needs GameScene defined
class GameStateSnapshot;

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
				
	virtual ~GameScene();

	//making public for saving state
	int _currentFrame;

	set<Orb*> _orbs;
	set<Spark*> _sparks;

	void cleanup();

private:
	CCSpriteBatchNode* _batchNode;
			
	bool _isManipulatingViewport;
	bool _isManipulatingSparks;
	CCPoint _prevViewporCenter;
	CCPoint _viewportDragVelocity;
	double _lastTouchBeganMillis;
	double _lastTouchEndedMillis;
	
	list<CCPoint> _prevTouches;
	list<CCPoint> _currentTouches;
	
	list<PingLocation*> _pingLocations;
	set<Spark*> _selectedSparks;
	void deselectSpark(Spark* spark);
	
	CCLayer* _gameLayer;
	
	void updateSparkSelectionEffects();
	void clearSelectedSparksIfNoAction();
	
	
	void draw();
	void update(float dt);
	void singleUpdateStep(float dt);

	void clearPingLocations();
	
	float _fixedTimestepAccumulator;

	list<GameStateSnapshot*> _gameStateSnapshots;
	bool _isRestoringGameStateSnapshot;
	bool _isCreatingGameStateSnapshot;
	void restoreToFrame(int targetFrame);
	void createGameStateSnapshot();
	
	list<Command*> _commandHistory;
};

#endif // __GAME_SCENE_H__
