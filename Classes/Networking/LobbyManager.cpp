//
//  LobbyManager.cpp
//  LightSwarm
//
//  Created by Stephen Johnson on 12/7/12.
//
//

#include "LobbyManager.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//TODO: this class should choose different methods to call based upon the platform being used



void LobbyManager::disconnect() {

	if(_sockfd == 0) {
		return;
	}
	
	close(_sockfd);
	_sockfd = 0;

	if(_delegate != NULL) {
		_delegate->onSuccess(DISCONNECT, "success!!!");
	}
}


string LobbyManager::getSignedToken() {



	return NULL;
}


void LobbyManager::identify(const string& username, const string& targetPlayer) {



}


LobbyManager::~LobbyManager() {
	_delegate = NULL;
	disconnect();
	
}










/*
// Socket server example

	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	const int port = 3001;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		CCLOGERROR("ERROR opening socket");
	}
	CCLOG("Opened socket");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR on binding");
	}
	
	listen(sockfd, 5);
	CCLOG("Bound to port %d", port);
	
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) {
		CCLOGERROR("ERROR on accept");
	}
	CCLOG("Accepted new socket connection");
	
	bzero(buffer, 256);
	n = read(newsockfd, buffer, 255);
	if (n < 0) {
		CCLOGERROR("ERROR reading from socket");
	}
	CCLOG("Received: %s\n", buffer);
	
	n = write(newsockfd, "I got your message", 18);
	if (n < 0) {
		CCLOGERROR("ERROR writing to socket");
	}
	
	close(newsockfd);
	close(sockfd);	
*/



/*
//Socket client example

	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	const int port = 3001;
	char buffer[256];
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		CCLOGERROR("ERROR opening socket");
	}
	
	server = gethostbyname("localhost");
	if (server == NULL) {
		CCLOGERROR("ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(port);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		CCLOGERROR("ERROR connecting");
	}
		
	bzero(buffer,256);
	strcpy(buffer, "test message");
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) {
		CCLOGERROR("ERROR writing to socket");
	}
	
	//bzero(buffer,256);
	//n = read(sockfd,buffer,255);
	//if (n < 0) {
	//	CCLOGERROR("ERROR reading from socket");
	//}
	//CCLOG("%s\n",buffer);
	close(sockfd);
*/

