//
//  NetworkedOpponent.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#include "NetworkedOpponent.h"









NetworkedOpponent::~NetworkedOpponent() {
	if(_socket != NULL) {
		delete _socket;
		_socket = NULL;
	}
}