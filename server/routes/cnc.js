var express = require('express');
var router = express.Router();
var com = require('./com.js');
var bodyParser = require('body-parser');

router.use(bodyParser.urlencoded({ extended: false }));
router.use(bodyParser.json());

router.post('/command', function (req, res) {
  //com(JSON.stringify(req.body), function(feedback){
    //res.json(feedback);
  //});
  res.json(req.body);
});

module.exports = router;
