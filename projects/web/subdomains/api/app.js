/***************** Dependencies and Initialization *****************/
var express = require('express');
var app = express();




/***************** Logging *****************/
app.all('*', function(req, res, next){
  console.log(req.ip + " " + req.path);
  next();
});

/***************** Bad Request Handling *****************/
app.all('/', function(req, res){
	res.redirect('http://www.LightSwarm.com');
});




/***************** Request Handling *****************/
app.all('/:objType/:action/:id', function(req, res){
  res.send('Hello World - ' + req.params.objType + "." + req.params.action + "." + req.params.id + "\n");
});






/***************** Socket Bind *****************/
app.enable('trust proxy')
app.listen(3000);
console.log('Listening on port 3000');

/***************** Error Handling *****************/
app.use(function(err, req, res, next){
  console.error(err.stack);
  res.send(500, 'Something broke!');
});
