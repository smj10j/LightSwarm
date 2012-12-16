//
//  NetworkedOpponent.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#ifndef __LightSwarm__NetworkedOpponent__
#define __LightSwarm__NetworkedOpponent__


#include "Socket.h"
#include "Opponent.h"
USING_NS_CC;
using namespace std;

class NetworkedOpponent : Opponent
{
public:

	NetworkedOpponent(Socket* socket):
		_socket(socket) {
		
	}
	
	virtual ~NetworkedOpponent();


	Socket* _socket;

protected:


private:

};


#endif /* defined(__LightSwarm__NetworkedOpponent__) */
