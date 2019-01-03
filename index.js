var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var output1 = {state:true};
var output2 = {state:true};
var mode = {state:'auto'};

app.get('/', function(req, res) {
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket) {
  console.log('User connected: ' + socket.id);
  //socket.emit('light', light);
  socket.on('disconnect', function(){
    console.log('User disconnected: ' + socket.id);
  });
  socket.on('output1', function(object) {
    console.log("Output1: " + object.state);
    io.sockets.emit('output1', object);
  });
  socket.on('output2', function(object) {
    console.log("Output2: " + object.state);
    io.sockets.emit('output2', object);
  });
  socket.on('mode', function(object) {
    console.log("Mode: " + object.state);
    console.log(object);
  });
});

http.listen(3000, function() {
  console.log('listening on *:3000');
});
