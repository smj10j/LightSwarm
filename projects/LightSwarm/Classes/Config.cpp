//
//  Config.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/3/12.
//
//

#include "Config.h"
#include <fstream>

const char* CONFIG_STRINGS[] = {
	stringify(MAX_TOUCHES),

	stringify(SPARK_BASE_SPEED),
	stringify(SPARK_BASE_POWER),
	stringify(SPARK_BASE_HEALTH),

	stringify(SPARK_HEALTH_COST_PER_SECOND_WHEN_TRAVELING),
	
	stringify(SPARK_PATH_SAMPLE_RATE),
	
	stringify(ORB_ATMOSPHERE_RADIUS_MULTIPLIER),
	stringify(ORB_BASE_HEAL_RATE_PER_SECOND),
	
	stringify(TOUCH_LASSO_BEGAN_DELAY_MILLIS),
	stringify(TOUCH_MOVE_BEGAN_DELAY_MILLIS),
	stringify(TOUCH_DOUBLE_TAP_DELAY_MILLIS),

	stringify(PING_LOCATION_SPINUP_SECONDS),
	stringify(PING_LOCATION_SPINDOWN_SECONDS),
};

static Json::Value root;   // will contains the root value after parsing.

static map<CONFIG,double> _doublesMap;
static map<CONFIG,int> _intsMap;

static double lastConfigReloadMillis = 0;

void Config::init() {
		
	//open up our config file
	ifstream is;
	const char* filepath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("GameConfig.json");
	is.open(filepath);
	
	if (is.is_open()) {
		root.clear();
		Json::Reader reader;	
		bool parsingSuccessful = reader.parse( is, root );
		if ( !parsingSuccessful ) {
			// report to the user the failure and their locations in the document.
			is.close();
			string msg = reader.getFormatedErrorMessages();
			CCLOG("Failed to parse configuration - %s", msg.c_str());
			exit(1);
		}
		is.close();
		
		_doublesMap.clear();
		_intsMap.clear();
	}
	
	lastConfigReloadMillis = Utilities::getMillis();
}

Json::Value Config::getValueForKey(CONFIG key) {
	if(MODIFYING_GAME_CONFIG && Utilities::getMillis()-lastConfigReloadMillis > 5000) {
		//constantly reload
		Config::init();
	}
	return root[CONFIG_STRINGS[key]];
}

double Config::getDoubleForKey(CONFIG key) {
	map<CONFIG,double>::iterator it = _doublesMap.find(key);
	if(it == _doublesMap.end()) {
		Json::Value value = Config::getValueForKey(key);
		if(value == NULL) return NULL;
		_doublesMap.insert(map<CONFIG,double>::value_type(key, value.asDouble()));
		return value.asDouble();
	}else {
		return it->second;
	}
}

int Config::getIntForKey(CONFIG key) {
	map<CONFIG,int>::iterator it = _intsMap.find(key);
	if(it == _intsMap.end()) {
		Json::Value value = Config::getValueForKey(key);
		if(value == NULL) return NULL;
		_intsMap.insert(map<CONFIG,int>::value_type(key, value.asInt()));
		return value.asInt();
	}else {
		return it->second;
	}
}
