var fs = require('fs');
var url = require('url');
var http = require('http');
var WebSocket = require('ws');
var querystring = require('querystring');
var db = [];
function requestHandler(request,response){
	var uriData = url.parse(request.url);
	var pathname = uriData.pathname;
	var query = uriData.query;
	var queryData = querystring.parse(query);

	if(pathname == '/update'){
		var newData = {
			temp: queryData.temp,
			humd: queryData.humd,
			time: new Date()
		};
		db.push(newData);
		console.log(newData);
		response.end();	
	}else if(pathname == '/get')
	{
		response.writeHead(200,{
			'Content-Type': 'application/json'
		});
		response.end(JSON.stringify(db));
		db = [];
	}else{
		fs.readFile('./index.html',function(error,content){
			response.writeHead(200,{
				'Content-Type' : 'text/html'
			});
			response.end(content);
		});
	}
}
var server = http.createServer(requestHandler);
var ws = new WebSocket.Server({server});
var clients = [];

function broadcast(socket,data){
	console.log(clients.length);
	for(i = 0;i<clients.length;i++){
		if(clients[i] != socket){
			clients[i].send(data);
		}
	}
}
ws.on('connection',function(socket,req){
	clients.push(socket);

	socket.on('message',function(message){
		console.log('recived : %s', message);
		broadcast(socket,message);
	});

	socket.on('close',function(){
		var index = clients.indexOf(socket);
		clients.splice(index,1);
		console.log('disconnectd');
	});
});
server.listen(8000);
console.log('server listening on port 8000');