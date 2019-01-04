var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
const request = require('request');
const fixieRequest = request.defaults({'proxy': process.env.FIXIE_URL});
var data = { output1: true, output2: true, mode: 'auto' };

app.get('/', function(req, res) {
  res.sendFile(__dirname + '/index.html');
});

// fixieRequest('http://superjunction.herokuapp.com/', (err, res, body) => {
//   console.log(`Got response: ${res.statusCode}`);
// });

io.on('connection', function(socket) {
  console.log('User connected: ' + socket.id);
  //socket.emit('light', light);
  socket.on('disconnect', function(){
    console.log('User disconnected: ' + socket.id);
  });
  socket.on('updateData', function(object){
    data = object;
    console.log("Update: ");
    console.log(data);
    io.sockets.emit('updateData', data);
  });
  socket.on("getData", function(){
    io.sockets.emit('updateData', data);
  });
});

http.listen(process.env.PORT, function() {
  console.log('listening on *:' + process.env.PORT);
});
