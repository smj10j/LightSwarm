//
//  Config.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/3/12.
//
//

#ifndef __LightSwarm__Config__
#define __LightSwarm__Config__


#include "Common.h"
#include <map>
#include "json.h"

using namespace std;


//BE SURE TO ADD THE RELEVANT stringify() command in Config.cpp!!!!
enum CONFIG {
	MAX_TOUCHES,
	
	SPARK_BASE_SPEED,
	SPARK_BASE_POWER,
	SPARK_BASE_HEALTH,
	
	SPARK_HEALTH_COST_PER_SECOND_WHEN_TRAVELING,
	
	SPARK_PATH_SAMPLE_RATE,
	
	ORB_ATMOSPHERE_RADIUS_MULTIPLIER,
	ORB_BASE_HEAL_RATE_PER_SECOND,
	
	TOUCH_LASSO_BEGAN_DELAY_MILLIS,
	TOUCH_MOVE_BEGAN_DELAY_MILLIS,
	TOUCH_DOUBLE_TAP_DELAY_MILLIS,
	TOUCH_MIN_PATH_POINT_DISTANCE,
	
	PING_LOCATION_SPINUP_SECONDS,
	PING_LOCATION_SPINDOWN_SECONDS,


	SIMULATION_STEP_SIZE,
	SIMULATION_MAX_STEPS,
	SIMULATION_FRAME_SIZE,
	SIMULATION_FRAME_STACK_SIZE
};
extern const char* CONFIG_STRINGS[];

class Config
{
public:

	static void init();

	static double getDoubleForKey(CONFIG key);
	static int getIntForKey(CONFIG key);
	
	
private:

	static Json::Value getValueForKey(CONFIG key);

};















#endif /* defined(__LightSwarm__Config__) */

