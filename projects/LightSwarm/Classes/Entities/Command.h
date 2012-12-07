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
#include <set>
#include <list>
USING_NS_CC;
using namespace std;

class Command
{
public:

	Command() {


	}

	virtual ~Command();


private:

	string _command;
	int _frame;
	list<Spark*> _sparks;
	list<Orb*> _orbs;

};
#endif /* defined(__LightSwarm__Command__) */
