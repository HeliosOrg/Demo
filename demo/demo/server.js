var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var Redis = require('ioredis');
var redis = new Redis();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true}));

redis.set('decalcolor', "");
redis.set('score', 0);
redis.set('clintonhit', 0);
redis.set('trumphit', 0);
redis.set('greenhit', 0);
redis.set('independenthit', 0);

app.get('/helios/decalcolor', function(req, res) { redis.get('decalcolor', function(err, result) { res.json({ value: result }); }); });
app.post('/helios/decalcolor', function(req, res) { redis.set('decalcolor', req.body.value, function(err, result) { res.json(true); }); });

app.get('/helios/score', function(req, res) { redis.get('score', function(err, result) { res.json({ value: JSON.parse(result) }); }); });
app.post('/helios/score', function(req, res) { redis.set('score', req.body.value, function(err, result) { res.json(true); }); });

app.get('/helios/clintonhit', function(req, res) { redis.get('clintonhit', function(err, result) { res.json({ value: JSON.parse(result) }); }); });
app.post('/helios/clintonhit', function(req, res) { redis.set('clintonhit', req.body.value, function(err, result) { res.json(true); }); });

app.get('/helios/trumphit', function(req, res) { redis.get('trumphit', function(err, result) { res.json({ value: JSON.parse(result) }); }); });
app.post('/helios/trumphit', function(req, res) { redis.set('trumphit', req.body.value, function(err, result) { res.json(true); }); });

app.get('/helios/greenhit', function(req, res) { redis.get('greenhit', function(err, result) { res.json({ value: JSON.parse(result) }); }); });
app.post('/helios/greenhit', function(req, res) { redis.set('greenhit', req.body.value, function(err, result) { res.json(true); }); });

app.get('/helios/independenthit', function(req, res) { redis.get('independenthit', function(err, result) { res.json({ value: JSON.parse(result) }); }); });
app.post('/helios/independenthit', function(req, res) { redis.set('independenthit', req.body.value, function(err, result) { res.json(true); }); });

app.get('/demo', function(req, res) {
  res.sendFile(__dirname + '/demo.html');
});

app.get('/score', function(req, res) {
  res.sendFile(__dirname + '/score.html');
});

app.listen(80);