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
#include "json.h"

using namespace std;

class Config
{

private:

	static Json::Value getValueForKey(string key);

public:

	static void init();

	static double getDoubleForKey(string key);
	static int getIntForKey(string key);
};









#define CONFIG_SPARK_BASE_SPEED "SPARK_BASE_SPEED" //TODO: need to figure out how to do this all with proper units
#define CONFIG_SPARK_BASE_POWER "SPARK_BASE_POWER"
#define CONFIG_SPARK_BASE_HEALTH "SPARK_BASE_HEALTH"

#define CONFIG_SPARK_HEALTH_COST_PER_SECOND_WHEN_TRAVELING "SPARK_HEALTH_COST_PER_SECOND_WHEN_TRAVELING"

#define CONFIG_SPARK_PATH_SAMPLE_RATE "SPARK_PATH_SAMPLE_RATE"

#define CONFIG_ORB_ATMOSPHERE_RADIUS_MULTIPLIER "ORB_ATMOSPHERE_RADIUS_MULTIPLIER"
#define CONFIG_ORB_BASE_HEAL_RATE_PER_SECOND "ORB_BASE_HEAL_RATE_PER_SECOND"

#define CONFIG_MAX_TOUCHES "MAX_TOUCHES"

#define CONFIG_VIEWPORT_SCALE_MIN "VIEWPORT_SCALE_MIN"
#define CONFIG_VIEWPORT_SCALE_MAX "VIEWPORT_SCALE_MAX"
#define CONFIG_VIEWPORT_SCALE_INITIAL "VIEWPORT_SCALE_INITIAL"











#endif /* defined(__LightSwarm__Config__) */

