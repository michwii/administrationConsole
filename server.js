var express = require('express');
var panTilt = require('./routes/panTilt');
var bodyParser = require('body-parser')
var app = express();

app.set('view engine', 'ejs');
app.use('/panTilt', panTilt);
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

app.get('/', function (req, res) {
  res.render('index');
})

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
})
