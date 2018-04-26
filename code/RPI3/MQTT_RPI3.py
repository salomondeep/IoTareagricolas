# Main template for our paho.mqtt.client code
# lets obtain potentiometer reading from ESP8266
# listen to topic "/esp/pot"
#!/usr/bin/env python
import paho.mqtt.client as mqtt

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))
	# Subscribing in on_connect() means that if we lose the connection and
	# reconnect then subscriptions will be renewed.
	client.subscribe("temp") # remember this topic to put inside ESP code later
	client.subscribe("humi")  # remember this topic to put inside ESP code later
	client.subscribe("move")  # remember this topic to put inside ESP code later

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload.decode()))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883, 60) # localhost is the Raspberry Pi itself

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()