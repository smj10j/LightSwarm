/***************** Dependencies and Initialization *****************/
var express = require('express');
var app = express();
var net = require('net');
var os = require('os');

var HTTP_PORT = 3000;
var GAME_PORT = 3001;
var MAX_CONNECTIONS = 20000;



var FRAME_SIZE = 50;
var MAX_EMPTY_FRAMES_BETWEEN_FLUSH = 20;
var NEXT_GAME_ID = 1;

var userToSocketMap = {};
var gamesMap = {};
var waitingFriendsToGameIds = {};
var waitingRandomUserIds = [];
var disconnectTimers = {};

/***************** HTTP Handling *****************/
app.all('*', function(req, res, next){
	console.log(req.ip + " " + req.path);
	res.redirect('http://www.LightSwarm.com');
});

app.enable('trust proxy')
app.listen(HTTP_PORT);
console.log('Listening on port '+HTTP_PORT+' for HTTP');

app.use(function(err, req, res, next){
  console.error(err.stack);
  res.send(500, 'Something broke!');
});
/***************** End HTTP Handling *****************/



/***************** Game Socket Setup *****************/

var onConnection = function(theSocket) {

	var socket = theSocket;
	var connectTime = (new Date()).getTime();
	socket.sessionId = "tcp."+connectTime+"."+(Math.random()+"").substr(2,6);
	
	console.log((new Date())+': '+ "Memory Usage: " + Math.ceil((os.freemem() / os.totalmem()) * 100) + '% free ('+(os.freemem()/(1024*1024))+'mb)');
	console.log((new Date())+': ' + "TCP Connections: " + gameLobbySocket.connections + " / " + gameLobbySocket.maxConnections);				

	//socket setup
	socket.setEncoding('utf8');	//use utf8
	socket.setTimeout(0);		//no timeout
	socket.setNoDelay(true);		//disable the Nagle algorithm
	socket.setKeepAlive(true);	//enable keep-alive
	
	log(socket, 'Connection established from ' + socket.remoteAddress);
	
	
	socket.publicPort = socket._peername.port;
	socket.publicIP = socket.remoteAddress;
	
	var onDisconnect = function() {
		var elapsedTime = (new Date()).getTime() - connectTime;
		log(socket, "TCP Socket was connected for " + (Math.floor(elapsedTime/1000)) + " seconds");
		eventDisconnect(socket);
	};
		
	var message = "";
	var messageLastReviewedIndex = 0;
	var messageObjectOpenCount = 0;
	var isParsingMessage = false;
	
	var parseMessage = function() {
	
		if(isParsingMessage) {
			return;
		}
		isParsingMessage = true;
		
		var foundMessage = false;
	
		for(messageLastReviewedIndex; messageLastReviewedIndex < message.length; messageLastReviewedIndex++) {
			var ch = message[messageLastReviewedIndex];
			//console.log('messageLastReviewedIndex: ' + messageLastReviewedIndex + ', ch='+ch);
			if(ch == '{') {
				messageObjectOpenCount++;
				//console.log((new Date())+': OPEN messageLastReviewedIndex='+messageLastReviewedIndex+', messageObjectOpenCount='+messageObjectOpenCount);
				
			}else if(ch == '}') {
				messageObjectOpenCount--;
				//console.log((new Date())+': CLOSE messageLastReviewedIndex='+messageLastReviewedIndex+', messageObjectOpenCount='+messageObjectOpenCount);
				
				if(messageObjectOpenCount == 0) {
					//found a complete object
					
					var singleMessage = message.slice(0, messageLastReviewedIndex+1);
					try {
						var messageObj = JSON.parse(singleMessage);
						message = message.slice(messageLastReviewedIndex+1);
						foundMessage = true;
		
					}catch(err) {
						log(socket, "Parsing error: " + JSON.stringify(err.message) + ", message: " + singleMessage);
						message = "";
					}
					
					if(foundMessage) {
						try {
							onMessage(socket, messageObj);
						}catch(err) {
							log(socket, "Message handling error: " + JSON.stringify(err.message) + ", message: " + JSON.stringify(messageObj));					
						}
					}
					
					messageLastReviewedIndex = 0;
					break;
									
				}else if(messageObjectOpenCount < 0) {
					//error
					log(socket, "Incorrectly formatted message: " + message);
					message = "";
					messageLastReviewedIndex = 0;
					messageObjectOpenCount = 0;
					break;
				}
			}
		}
				
		isParsingMessage = false;
		if(foundMessage) {
			parseMessage();
		}
	};
	
	//handle/parse the data
	socket.on('data', function(data) {
		message+= data.trim();
		parseMessage();
	});
	
	socket.on('close', function(withError) {
		onDisconnect();
		log(socket, "Closed " + (withError ? "with ERROR" : "gracefully"));		
	});
	
	socket.on('timeout', function() {
		onDisconnect()
		log(socket, "Timed out due to inactivity");		
	});
	
	socket.on('end', function() {
		log(socket, "Received a FIN packet");		
	});
	
	socket.on('error', function(err) {
		log(socket, "Errored: " + err.message);
		if(err.stack) {
		  console.log('\nStacktrace:')
		  console.log('====================')
		  console.log(err.stack);
		}
		onDisconnect();
	});
};



var gameLobbySocket = net.createServer();
gameLobbySocket.maxConnections = MAX_CONNECTIONS;
gameLobbySocket.on('connection', onConnection);
gameLobbySocket.listen(GAME_PORT);
console.log('Listening on port '+GAME_PORT+' for game socket');

/***************** End Game Socket Setup *****************/







/***************** Game Socket Message Handling *****************/

var validateSignature = function(socket, message) {

	var signature = message._sig;
	//TODO: validate signature once client side is complete (too annoying manually)
	return;

	//unauthorized message

	throw {
		error: {
			code: 501,
			message: "Invalid signature"
		},
		forceDisconnect: true
	};
};

var onMessage = function(socket, message) {
	
	log(socket, "Handling message " + JSON.stringify(message));

	//all requests must be signed by our shared secret in the app
	validateSignature(socket, message);

	//default response
	var response = {
		status: 'ok'
	};


	try {
		if(message.action == 'identify') {			
	
			if(!socket.user) {	
				socket.user = message.user;
				log(socket, "Set user: " + JSON.stringify(socket.user));
				
				socket.privateIP = message.privateIP;
				socket.privatePort = message.privatePort;
				
				log(socket, "Set private address "+socket.privateIP+":"+socket.privatePort);
				
				clearTimeout(disconnectTimers[socket.user.userId]);
				delete disconnectTimers[socket.user.userId]; 
			}
	
		}else if(message.action == 'setPrefs') {			
	
			socket.user.prefs = message.prefs;
			log(socket, "Set prefs: " + JSON.stringify(socket.user.prefs));
			
			if(socket.user.prefs.reconnect) {
				log(socket, "Wants to reconnect");
				stateReconnect(socket);
				
			}else {
				//create player
				eventConnect(socket);
				
				//add into the game (or create if not-exist)
				stateJoinGame(socket);
			}
	
		}else if(message.action == 'command') {			
	
			actionCommand(socket, message);
			
		}else if(message.action == 'forfeit') {			
	
			actionForfeit(socket, message);
			
		}else if(message.action == 'win') {			
	
			actionWin(socket, message);
			
		}else if(message.action == 'loss') {			
	
			actionLoss(socket, message);
			
		}else {
			console.log("Unknown request action: " + message.action);
			response.status = 'error';
			response.error = {
				code: 500,
				message: "Unknown request action: " + message.action
			}
		}	
	}catch(err) {
	
		if(err.error) {
			console.log("Returning error: " + JSON.stringify(err.error));
			response.status = 'error';
			response.error = err.error;
			if(err.forceDisconnect) {
				response.forceDisconnect = true;
			}
		}else {
			//parse error or js error in general
			throw err;
		}
		
	}finally {
		//respond
		response = JSON.stringify(response) + "~|~";
		if(response.forceDisconnect) {
			socket.end(response);
		}else {
			socket.write(response);	
		}		
	}
				
};



/***************** End Game Socket Message Handling *****************/






/***************** Game State Handling *****************/



//creating a new game
var stateCreateGame = function(socket) {
	log(socket, "State Create Game");

	var gameId = ''+NEXT_GAME_ID++;
	socket.user.gameId = gameId;
	
	var seed = 100000*Math.random();
	for(var i = 0; i < Math.random()*10; i++) {
		seed+= 100000*Math.random()
	}
	seed = Math.floor(seed);
	
	var game = {
		gameId: gameId,
		player1Id: socket.user.userId,
		player2Id: null,
		seed: seed,
		startTime: null,
		commandQueue: []
	};
	gamesMap[gameId] = game;
	
	log(socket, "Created game: " + JSON.stringify(game));

	if(socket.user.prefs.playWithFriend) {
		waitingFriendsToGameIds[socket.user.userId] = gameId;
	}else {
		waitingRandomUserIds.push(socket.user.userId);
	}
	

	//dumpGameState();
};



//trying to find the game
var stateJoinGame = function(socket) {
	log(socket, "State Join Game");
	
	if(socket.user.prefs.playWithFriend) {
		//gamecenter friend
		
		var gameId = waitingFriendsToGameIds[socket.user.prefs.playWithFriend];
		if(gameId) {
			//found
			
			//remove from waiting queue
			delete waitingFriendsToGameIds[socket.user.prefs.playWithFriend];
			
			//add user to game
			var game = gamesMap[gameId];
			game.player2Id = socket.user.userId;
			
			//start!
			stateStartGame(game);
			
		}else {
			//not found - create a game!
			stateCreateGame(socket);
		}
		
	}else {
		//random
	
		var waitingUserId = waitingRandomUserIds[0];
		if(waitingUserId) {
			//found
			
			//remove from waiting queue
			waitingRandomUserIds.splice(0,1);
			log(socket, "Found waitingUserId="+waitingUserId);
			
			//add user to game
			var waitingSocket = userToSocketMap[waitingUserId];
			var game = gamesMap[waitingSocket.user.gameId];
			game.player2Id = socket.user.userId;
			
			//start!
			stateStartGame(game);
			
		}else {
			//not found - create a game!
			stateCreateGame(socket);		
		}
	
	}
};


//initializing the game
var stateStartGame = function(game) {

	log(game, "State Start Game");

	var player1 = userToSocketMap[game.player1Id];	
	var player2 = userToSocketMap[game.player2Id];	

	//choose a start time (5 seconds from now)
	game.startTime = (new Date()).getTime() + 5000;

	//TODO: test udp-hole punching and set canBeServer appropriately
	//TODO: set connectToPeer to false for both if canBeServer is false for player1 and player2
	var connectToPeer = true;
	
	//notify players of the start time, and the seed
	var response = {
		status:'ok',
		gameInit: {
			seed: game.seed,
			player1: {
				userId: player1.user.userId,
				publicIP: player1.publicIP,
				publicPort: player1.publicPort,
				privateIP: player1.privateIP,
				privatePort: player1.privatePort,
				canBeServer: true,
				connectToPeer: connectToPeer
			},
			player2: {
				userId: player2.user.userId,
				publicIP: player2.publicIP,
				publicPort: player2.publicPort,
				privateIP: player2.privateIP,
				privatePort: player2.privatePort,
				canBeServer: true,
				connectToPeer: connectToPeer
			}
		}
	};
	player1.write(JSON.stringify(response));
	player2.write(JSON.stringify(response));
	 
	 
	
	if(connectToPeer) {

		//disconnct players now
		actionDisconnect(player1, "Peer-to-peer connection started");	
		
	}else {
		 
		//start the command flush queue	
		game.lastFlushTimestamp = 0;
		game.flushCommandQueueTimer = setInterval(function() {
			actionFlushCommandQueue(game);
		}, FRAME_SIZE*MAX_EMPTY_FRAMES_BETWEEN_FLUSH);

		log("Command queue started");
	}

	//and we're off!
};



//flushing the command queue was requested
var actionFlushCommandQueue = function(game) {
	
	var now = (new Date()).getTime();
	if(now - game.lastFlushTimestamp < FRAME_SIZE) {
		return;
	}
	game.lastFlushTimestamp = now;

	log(game, "Action Flush Command Queue");

	var player1 = userToSocketMap[game.player1Id];	
	var player2 = userToSocketMap[game.player2Id];	

	//write out the command queue to both users
	var commandQueue = [];
	for(var i in game.commandQueue) {
		commandQueue.push(game.commandQueue[i]);
	}
	game.commandQueue = [];
	
	var response = {
		status:'ok',
		commandQueue: {
			commandQueue: commandQueue
		}
	};
	player1.write(JSON.stringify(response));
	player2.write(JSON.stringify(response));	
	
};




//incoming command
var actionCommand = function(socket, message) {
	log(socket, "Action Command");

	var game = gamesMap[socket.user.gameId];

	var command = message.command;
	command.userId = socket.user.userId;
	command.frame = Math.floor(((new Date()).getTime() - game.startTime)/FRAME_SIZE);
	
	game.commandQueue.push(command);

	setTimeout(function() {
		actionFlushCommandQueue(game);	
	}, FRAME_SIZE);
};

//incoming forfeit message
var actionForfeit = function(socket, message) {
	log(socket, "Action Forfeit");

	actionDisconnect(socket, "Forfeit");
};


//incoming win message
var actionWin = function(socket, message) {
	log(socket, "Action Win");

	actionDisconnect(socket, "Win");
};

//incoming lost message
var actionLoss = function(socket, message) {
	log(socket, "Action Loss");

	actionDisconnect(socket, "Loss");
};




//user has connected to the server and wishes to join a game
var eventConnect = function(socket) {

	//TODO: score data should be stored in a database (along with info like # plays, game histories, etc.)

	if(!socket.user) {
		socket.user = {
			gameId: null,
			score: 1000
		};
	}
	userToSocketMap[socket.user.userId] = socket;
};

//user-requested reconnect attempt
var stateReconnect = function(socket) {
	log(socket, "State Reconnect");

	if(userToSocketMap[socket.user.userId]) {
		//user still seen as being online - yay
		
		//update references
		var oldSocket = userToSocketMap[socket.user.userId];
		socket.user = oldSocket.user;
		
		var game = gamesMap[socket.user.gameId];
		if(game) {
			//game still available
								
		}else {
			throw {
				error: {
					code: 206,
					message: "Game no longer exists"
				}
			};		
		}
		
	}else {
		//user already cleared from server
		throw {
			error: {
				code: 205,
				message: "Game no longer exists"
			}
		};
	}
};

//a disconnect was detected
var eventDisconnect = function(socket) {
	
	if(socket.forceDisconnect) {
		return;
	}
	
	if(!socket.user) {
		return;
	}

	if(disconnectTimers[socket.user.userId]) {
		//already handling
		return;
	}

	log(socket, "Event Disconnect");
	
	//start a timer to end the game
	disconnectTimers[socket.user.userId] = setTimeout(function() {
		actionDisconnect(socket, "Disconnect Timer");
	}, 15000);
	
	if(socket.user && socket.user.gameId) {
		delete gamesMap[socket.user.gameId];
	}
};

//a quit game request was made by the game or one of the players
var actionDisconnect = function(socket, reason) {	
	log(socket, "Action Disconnect - " + reason);
	
	var game = socket && gamesMap[socket.user.gameId];
	var opponentSocket = game && (game.player1Id == socket.user.userId ? userToSocketMap[game.player2Id] : userToSocketMap[game.player1Id]);

	//cleanup
	opponentSocket && clearTimeout(disconnectTimers[opponentSocket.user.userId]);
	opponentSocket && delete disconnectTimers[opponentSocket.user.userId];
	socket && clearTimeout(disconnectTimers[socket.user.userId]);
	socket && delete disconnectTimers[socket.user.userId];
	game && clearTimeout(game.flushCommandQueueTimer);

	game && delete userToSocketMap[game.player1Id];
	game && delete userToSocketMap[game.player2Id];

	opponentSocket && delete userToSocketMap[opponentSocket.user.userId];
	opponentSocket && delete waitingFriendsToGameIds[opponentSocket.user.userId];
	socket && delete waitingFriendsToGameIds[socket.user.userId];
	socket && delete userToSocketMap[socket.user.userId];
	socket && delete gamesMap[socket.user.gameId];
	
	//flag as manually disconnected
	if(socket) socket.forceDisconnect = true;
	if(opponentSocket) opponentSocket.forceDisconnect = true;
	
	
	for(var i in waitingRandomUserIds) {
		if(socket && socket.user.userId == waitingRandomUserIds[i]) {
			waitingRandomUserIds.splice(i,1);
		}else if(opponentSocket && opponentSocket.user.userId == waitingRandomUserIds[i]) {
			waitingRandomUserIds.splice(i,1);
		}
	}
	
	socket && socket.end('{}'+'~|~');
	opponentSocket && opponentSocket.end('{}'+'~|~');
		
	
	//TODO: log the information to database somewhwere
	
	dumpGameState();
};


/***************** End Game State Handling *****************/














/***************** Utilities *****************/

var log = function(object, message) {
	if(object.user) {
		//socket
		console.log((new Date())+': socket ' + object.sessionId + ', User ' + (object.user ? object.user.userId : 'unknown') + ': ' + message);
	}else if(object.gameId) {
		//game
		console.log((new Date())+': game ' + object.gameId + ': ' + message);
	}else {
		console.log((new Date())+': ' + message);	
	}
};

var dumpGameState = function() {
	console.log('======== DUMP GAME STATE ============')

	console.log('Users ============')
	for(var userId in userToSocketMap) {
		var socket = userToSocketMap[userId];
		console.log(userId + " => " + JSON.stringify(socket.user));
	}
	
	console.log('Games ============')
	console.log(JSON.stringify(gamesMap));

	console.log('Waiting Friends ============')
	console.log(JSON.stringify(waitingFriendsToGameIds));

	console.log('Waiting Randoms ============')
	console.log(JSON.stringify(waitingRandomUserIds));

	console.log('======== END DUMP GAME STATE ============')

}

/***************** End Utilities *****************/





/***************** Uncaught Error Handling *****************/

process.on('uncaughtException', function (err) {
  dumpError(err);
});

var dumpError = function(err) {
  if (typeof err === 'object') {
    if (err.message) {
      console.log('\nEXCEPTION: ' + err.message + '\n')
    }
    if (err.stack) {
      console.log('\nStacktrace:')
      console.log('====================')
      console.log(err.stack);
      console.log('====================')
   }
  } else {
    console.log('\nEXCEPTION dumpError :: argument is not an object\n');
  }
};
/***************** End Uncaught Error Handling *****************/
