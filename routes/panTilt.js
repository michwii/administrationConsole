var express = require('express');
var router = express.Router();
var com = require('./com.js');


router.post('/gripper', function (req, res) {
  com(JSON.stringify({gripperMove:1}));
  res.json({gripperMove:1});
});
router.post('/pan', function (req, res) {
  com(JSON.stringify({panMove:1}));
  res.json({panMove:1});
});
router.post('/tilt', function (req, res) {
  com(JSON.stringify({tiltMove:1}));
  res.json({tiltMove:1});
});

module.exports = router;
