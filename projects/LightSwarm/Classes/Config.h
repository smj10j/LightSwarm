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

class Config
{
public:

	static void init();

	static double getDoubleForKey(string key);
	static int getIntForKey(string key);
	
	
private:

	static Json::Value getValueForKey(string key);

};









#define CONFIG_SPARK_BASE_SPEED "SPARK_BASE_SPEED" //TODO: need to figure out how to do this all with proper units
#define CONFIG_SPARK_BASE_POWER "SPARK_BASE_POWER"
#define CONFIG_SPARK_BASE_HEALTH "SPARK_BASE_HEALTH"

#define CONFIG_SPARK_HEALTH_COST_PER_SECOND_WHEN_TRAVELING "SPARK_HEALTH_COST_PER_SECOND_WHEN_TRAVELING"

#define CONFIG_SPARK_PATH_SAMPLE_RATE "SPARK_PATH_SAMPLE_RATE"

#define CONFIG_ORB_ATMOSPHERE_RADIUS_MULTIPLIER "ORB_ATMOSPHERE_RADIUS_MULTIPLIER"
#define CONFIG_ORB_BASE_HEAL_RATE_PER_SECOND "ORB_BASE_HEAL_RATE_PER_SECOND"

#define CONFIG_MAX_TOUCHES "MAX_TOUCHES"

#define CONFIG_TOUCH_LASSO_BEGAN_DELAY_MILLIS "TOUCH_LASSO_BEGAN_DELAY_MILLIS"
#define CONFIG_TOUCH_MOVE_BEGAN_DELAY_MILLIS "TOUCH_MOVE_BEGAN_DELAY_MILLIS"
#define CONFIG_TOUCH_DOUBLE_TAP_DELAY_MILLIS "TOUCH_DOUBLE_TAP_DELAY_MILLIS"

#define CONFIG_PING_LOCATION_SPINUP_SECONDS "PING_LOCATION_SPINUP_SECONDS"
#define CONFIG_PING_LOCATION_SPINDOWN_SECONDS "PING_LOCATION_SPINDOWN_SECONDS"








#endif /* defined(__LightSwarm__Config__) */

