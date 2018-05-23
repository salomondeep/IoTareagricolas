
var Hapi = require('hapi');
var mqtt = require('mqtt');

var server = new Hapi.Server();
var port = Number(process.env.PORT || 4444);

server.connection({ port: port, routes: { cors: true } });

var client  = mqtt.connect('mqtt://localhost:1883');

var mqttPublish = function(topic, msg){
  client.publish(topic, msg, function() {
    console.log('msg sent: ' + msg);
  });
}

server.route([
  {
    method: 'POST',
    path: '/',
    handler: function (request, reply) {
      reply('Arrived here');
      mqttPublish('node', 'status', {
        'qos' : 2
      });
      console.log('Sent package');
    }
  }
]);

server.start();
