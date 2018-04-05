var express = require('express')
var app = express();
var bodyParser = require('body-parser');
var path = require('path')
var server = require('http').createServer(app);
var io = require('socket.io')(server);
var db = [];
var logined = false;
var btnStatus = {
  led: false,
  led2: false,
  btn: false
}

app.set('views', './views');
app.set('view engine', 'hbs');
app.use(express.static(path.join(__dirname, 'public')));

// support parsing of application/json type post data
app.use(bodyParser.json());

//support parsing of application/x-www-form-urlencoded post data
app.use(bodyParser.urlencoded({ extended: true }));

app.get('/', function (req, res,next) {
  if(!logined) {
    res.redirect('/login')
  } else {
    res.render('index')
  }
});
app.get('/login',function(req,res) {
  res.render('login')
})
app.post('/login', function(req,res) {
  let loginData = req.body;
  if(loginData.username === 'admin' && loginData.password === 'admin') {
    logined = true;
    res.redirect('/')
  } else {
    res.status(400).json('Login failed')
  }
})

app.get('/update', function(req,res) {
  var data = {
    temp: req.query.temp,
    humd: req.query.humd,
    time: new Date()
  }
  db.push(data)
  io.emit('UPDATE', data);
  res.json(db)
})

io.on('connection', function (socket) {
  socket.on('INIT' , function() {
    socket.emit('LOAD DB', db);
    socket.emit('LOAD STATUS', btnStatus)
    console.log('new connection');
  })
  socket.on('BTN_CHANGE_CLIENT', function(status) {
    switch(status) {
      case 'BTN_PRESSED': btnStatus.btn = true; break;
      case 'BTN_RELEASE': btnStatus.btn = false; break;
      case 'LED1_OFF': btnStatus.led = false; break;
      case 'LED1_ON': btnStatus.led = true; break;
      case 'LED2_OFF': btnStatus.led2 = false; break;
      case 'LED2_ON': btnStatus.led2 = true; break;
    }
    io.emit('BTN_CHANGE_SERVER', status);
  })
});

var updateInterval = 2000; // Thời gian cập nhật dữ liệu 2000ms = 2s
setInterval(function(){ 
  io.emit('UPDATE', db[db.length-1]); 
}, updateInterval);

server.listen(process.env.PORT || 8000);