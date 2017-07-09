var SerialPort = require('serialport');
var callback;
var port = new SerialPort('COM3', {
  parser: SerialPort.parsers.readline('\n'),
  baudRate: 250000
});

port.on('open', function() {

});

port.on('data', function (data) {
  callback(JSON.parse(data));
});

// open errors will be emitted as an error event
port.on('error', function(err) {
  console.log('Error: ', err.message);
});

module.exports = function sendMessage(message, cb){
  port.write(message, function(err) {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
  });
  callback = cb;
}
