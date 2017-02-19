var SerialPort = require('serialport');
var express = require('express');
var app = express();

app.set('view engine', 'ejs');

var port = new SerialPort('COM3', {
  parser: SerialPort.parsers.readline('\n'),
  baudRate: 9600
});

port.on('open', function() {

});
// open errors will be emitted as an error event
port.on('error', function(err) {
  console.log('Error: ', err.message);
});


app.get('/', function (req, res) {
  res.render('index');
})

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
})
