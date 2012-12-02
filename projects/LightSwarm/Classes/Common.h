//
//  Constants.h
//  LightSwarm
//
//  Created by Stephen Johnson on 11/27/12.
//
//

#ifndef LightSwarm_Constants_h
#define LightSwarm_Constants_h



/******************* USER-DEFINABLE ******************/

//prepares all settings for App Store
//NOTE: be sure to set the correct distribution provisioning profile!
#define DISTRIBUTION_BUILD false

//enables TestFlight as well as sets this as a Distribution build
//NOTE: be sure to set the correct distribution provisioning profile!
#define BETA_BUILD (false && !DISTRIBUTION_BUILD)

/******************* END USER-DEFINABLE ******************/







//true to disable all output and send analytics data (DON'T SET THIS DIRECTLY)
#define DEVICE_BUILD (false || BETA_BUILD || DISTRIBUTION_BUILD)






 
/******************* USER-DEFINABLE ******************/

#define TARGET_FPS 60

#define DEBUG_ALL_THE_THINGS	( false							 && !DEVICE_BUILD)
#define DEBUG_ABC				((false || DEBUG_ALL_THE_THINGS) && !DEVICE_BUILD)


#define MAX_TOUCHES 400

#define VIEWPORT_SCALE_MIN 0.25
#define VIEWPORT_SCALE_INITIAL 1.0
#define VIEWPORT_SCALE_MAX 2


//TODO: move all game config items into an xml file that can be reloaded at run-time
//this will speed up balance and gameplay adjustments by not having to recompile



/******************* END USER-DEFINABLE ******************/






#if DEVICE_BUILD
#define COCOS2D_DEBUG 0
#else
#define COCOS2D_DEBUG 1
#endif




// Common includes
#include "cocos2d.h"
#include "AppMacros.h"
#include "Utilities.h"
#endif

