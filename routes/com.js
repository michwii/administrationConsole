var SerialPort = require('serialport');

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

module.exports = function sendMessage(message){
  port.write(message, function(err) {
    if (err) {
      return console.log('Error on write: ', err.message);
    }
  });

}
