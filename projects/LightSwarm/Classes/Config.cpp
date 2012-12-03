//
//  Config.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/3/12.
//
//

#include "Config.h"
#include <fstream>

static Json::Value root;   // will contains the root value after parsing.
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
	}
	lastConfigReloadMillis = Utilities::getMillis();
}

Json::Value Config::getValueForKey(string key) {
	if(MODIFYING_GAME_CONFIG && Utilities::getMillis()-lastConfigReloadMillis > 5000) {
		//constantly reload
		Config::init();
	}
	return root[key];
}

double Config::getDoubleForKey(string key) {
	Json::Value value = Config::getValueForKey(key);
	if(value == NULL) return NULL;
	return value.asDouble();
}

int Config::getIntForKey(string key) {
	Json::Value value = Config::getValueForKey(key);
	if(value == NULL) return NULL;
	return value.asInt();
}
