#include "AppDelegate.h"
#include "GameScene.h"
#include "LobbyScene.h"
#include "Common.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto eglView = EGLView::getInstance();
    
    director->setOpenGLView(eglView);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / TARGET_FPS);
    
    if(!DEVICE_BUILD) {
		// turn on display FPS
		director->setDisplayStats(true);
	}
	
	//default blend function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	//setup commonly used utils
	Utilities::init();
	Config::init();
    
    // create a scene. it's an autorelease object
    auto scene = LobbyScene::scene();
    
    // run
    director->runWithScene(scene);
    
    return true;
    
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}