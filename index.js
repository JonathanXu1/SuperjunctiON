var app = require('express')();
var http = require('http').Server(app);
var http1 = require('http');
var io = require('socket.io')(http);
var url = require("url");
var data = { output1: true, output2: true, mode: 'auto' };

var proxy = url.parse(process.env.QUOTAGUARDSTATIC_URL);
var target  = url.parse("https://superjunction.herokuapp.com/");

var cust;

var options = {
  hostname: proxy.hostname,
  port: proxy.port || 5000,
  path: target.href,
  headers: {
    "Proxy-Authorization": "Basic " + (new Buffer(proxy.auth).toString("base64")),
    "Host" : target.hostname
  }
};

http1.get(options, function(res) {
  res.pipe(process.stdout);
  return console.log("status code", res.statusCode);
  cust = res.statusCode;
});

app.get('/', function(req, res) {
  res.sendFile(__dirname + '/index.html');
});

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
    io.sockets.emit('ipAddress', {ip:cust});
  });
});

http.listen(process.env.PORT, function() {
  console.log('listening on *:' + process.env.PORT);
});
