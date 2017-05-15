var express = require('express');
var router = express.Router();
var com = require('./com.js');
var bodyParser = require('body-parser');

router.use(bodyParser.urlencoded({ extended: false }));
router.use(bodyParser.json());

router.post('/X_axis', function (req, res) {
  com(JSON.stringify(req.body), function(feedback){
    res.json(feedback);
  });

});
router.post('/A_axis', function (req, res) {
  com(JSON.stringify(req.body), function(feedback){
    res.json(feedback);
  });
});
router.post('/Y_axis', function (req, res) {
  com(JSON.stringify(req.body), function(feedback){
    res.json(feedback);
  });
});

module.exports = router;
