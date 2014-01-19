//
//  Command.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/6/12.
//
//

#ifndef __LightSwarm__Command__
#define __LightSwarm__Command__


#include "Common.h"
#include "Spark.h"
#include "Orb.h"
#include "GameScene.h"
#include <set>
#include <list>
USING_NS_CC;
using namespace std;

enum COMMAND_TYPE {
	MOVE
};

enum COMMAND_ID_TYPE {
	ORB,
	SPARK
};

class Command
{
public:

	Command(COMMAND_TYPE command, int frame, COMMAND_ID_TYPE idType, list<int>& ids, list<CCPoint>& path):
		_command(command),
		_frame(frame),
		_idType(idType),
		_ids(ids),
		_path(path) {

		
	}

	virtual ~Command();


	COMMAND_TYPE _command;
	int _frame;
	COMMAND_ID_TYPE _idType;
	list<int> _ids;
	list<CCPoint> _path;

private:

};
#endif /* defined(__LightSwarm__Command__) */
