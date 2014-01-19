#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "NetworkedOpponent.h"
#include "PingLocation.h"
#include <list>
USING_NS_CC;
using namespace std;

//forward declaration and GameStateSnapshot needs GameScene defined
class GameStateSnapshot;
class Command;

class GameScene : public CCLayer, public SocketDelegate
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene* scene(Player* player, Opponent* opponent);
    static CCScene* scene(GameScene *layer);

    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);
	
	
	//touch handlers
	virtual void touchesMoved(CCSet* touches, CCEvent* event);
	virtual void touchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	virtual void touchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
		
	virtual void onEnter();
	virtual void onExit();
	
	virtual void onConnect();
	virtual void onDisconnect();
	virtual void onDisconnectChild();
	virtual void onMessage(const Json::Value& message);

				
	virtual ~GameScene();

	//making public for saving state
	int _currentFrame;
	list<Orb*> _orbs;
	list<Spark*> _sparks;


	Player* _player;
	Opponent* _opponent;
	
protected:
		
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
	void clearSelectedSparksIfNoAction(float dt);
	
	void cleanup();
	
	void draw();
	void update(float dt);
	void singleUpdateStep(float dt);

	void clearPingLocations();
	
	float _fixedTimestepAccumulator;

	list<GameStateSnapshot*> _gameStateSnapshots;
	bool _isRestoringGameStateSnapshot;
	bool _isCreatingGameStateSnapshot;
	void createGameStateSnapshot(float dt);
	void restoreToFrameAndExecuteCommand(int targetFrame, Command* command);
	
	list<Command*> _commandQueue;
	void processCommandQueue();
	void processCommand(Command* command);	//sets the current frame ready for execution of the command
	void executeCommand(Command* command);	//actually performs the action
};

#endif // __GAME_SCENE_H__
