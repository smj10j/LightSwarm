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

var playersMap = {};
var gamesMaps = {};



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
	console.log((new Date())+': ' + "TCP Connections: " + gameServerSocket.connections + " / " + gameServerSocket.maxConnections);				

	//socket setup
	socket.setEncoding('utf8');	//use utf8
	socket.setTimeout(0);		//no timeout
	socket.setNoDelay(true);		//disable the Nagle algorithm
	socket.setKeepAlive(true);	//enable keep-alive
	
	log(socket, 'Connection established from ' + socket.remoteAddress);
	
	var onDisconnect = function() {
		var elapsedTime = (new Date()).getTime() - connectTime;
		log(socket, "TCP Socket was connected for " + (Math.floor(elapsedTime/1000)) + " seconds");
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



var gameServerSocket = net.createServer();
gameServerSocket.maxConnections = MAX_CONNECTIONS;
gameServerSocket.on('connection', onConnection);
gameServerSocket.listen(GAME_PORT);
console.log('Listening on port '+GAME_PORT+' for game socket');

/***************** End Game Socket Setup *****************/







/***************** Game Socket Message Handling *****************/

var validateSignature = function(socket, message) {

	var signature = message._sig;
	//TODO: validate signature;
	return;

	//unauthorized message
	var response = {
		status: 'error',
		error: {
			code: 501,
			message: "Invalid signature"
		},
		message: message
	};
	response = (message._mid ? message._mid : '-1') + "|" + JSON.stringify(response);
				
	socket.end(response);

	throw {
		message: (new Date())+': ' + 'Client ' + socket.sessionId + ': Forcing a disconnect because the request was not correctly signed'
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



	if(message.action == 'identify') {			

		socket.user = message.user;
		log(socket, "Set user");

	}else {
		console.log("Unknown request action: " + message.action);
		response.status = 'error';
		response.error = {
			code: 500,
			message: "Unknown request action: " + message.action
		}
	}	

	//respond
	response = (message._mid ? message._mid : '-1') + "|" + JSON.stringify(response);
	socket.write(response);					
};

/***************** End Game Socket Message Handling *****************/








/***************** Utilities *****************/

var log = function(socket, message) {
	console.log((new Date())+': ' + 'socket ' + socket.sessionId + ', User ' + (socket.user ? socket.user.userId : 'unknown') + ': ' + message);
};

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
