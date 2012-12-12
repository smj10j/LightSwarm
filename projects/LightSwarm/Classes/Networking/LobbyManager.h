//
//  LobbyManager.h
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#ifndef __LightSwarm__LobbyManager__
#define __LightSwarm__LobbyManager__

#include "Common.h"
USING_NS_CC;
using namespace std;

enum RESPONSE_TYPE {
	CONNECT,
	DISCONNECT,
	MESSAGE
};

#define LOBBY_SERVER	"localhost"
#define LOBBY_PORT		3001


class LobbyManagerDelegate
{
public:
	virtual ~LobbyManagerDelegate() {};
	virtual void onSuccess(RESPONSE_TYPE type, string message) {};
	virtual void onError(RESPONSE_TYPE type, string error) {};
};

//TODO: a lot of this class's network logic should be refactored into a generic Socket class

class LobbyManager
{
public:

	LobbyManager(LobbyManagerDelegate* delegate):
		_delegate(delegate) {

		_sockfd = 0;
	}
	
	void setToken(string& token) {
		_token = token;
	}
	
	bool connect_();
	void disconnect();
	
	string getSignedToken();
	
	//identifies the user and sends if they want to connect to a specific person or a random
	//null for random
	void identify(const string& username, const string& targetPlayer);
	
	
	virtual ~LobbyManager();

private:

	LobbyManagerDelegate* _delegate;

	string _token;
};

#endif /* defined(__LightSwarm__LobbyManager__) */
