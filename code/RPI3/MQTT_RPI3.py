# Main template for our paho.mqtt.client code
# lets obtain potentiometer reading from ESP8266
# listen to topic "/esp/pot"
# !/usr/bin/env python
import paho.mqtt.client as mqtt
import serial
import requests
import os
from datetime import datetime


# The callback for when the client receives a CONNACK response from the server.

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("temp")  # remember this topic to put inside ESP code later
    client.subscribe("humi")  # remember this
    client.subscribe("move")  # remember this topic to put inside ESP code later
    client.subscribe("thingIp")  # remember this topic to put inside ESP code later
    client.subscribe("sensor")  # remember this topic to put inside ESP code later
    client.subscribe("lora_things")
    client.subscribe("lora_sensor")
    client.subscribe("lora_distance")
    client.subscribe("lora_gateway")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	if msg.topic == "humi":
		#Post here the humidity
		paramUrl = 'http://iotplatform.ess/api/postvalue'
		paramJson = {'value': str(msg.payload.decode()), 'name': 'humidity', 'sensorName':'DHT11'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(r) #Great success! ♥

	elif msg.topic == "temp":
        	#Post here the temperature

		paramUrl = 'http://iotplatform.ess/api/postvalue'
		paramJson = {'value': str(msg.payload.decode()), 'name': 'temperature', 'sensorName':'DHT11'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)

		print(r) #Great success! ♥

	elif msg.topic == "move":
		#Post here the movement

		paramUrl = 'http://iotplatform.ess/api/postmovement'
		paramJson = {'value': 1, 'name': 'QRE movement', 'sensorName':'QRE'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)

		print(r) #Great success! ♥

	elif msg.topic == "thingIp":
		#Post here the thing

		paramUrl = 'http://iotplatform.ess/api/postthinginfo'
		paramJson = {'value': str(msg.payload.decode()), 'thingName': 'nodemcu', 'designation': 'Thing instalada na estufa'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(str(msg.payload.decode()))
		print(r) #Great success! ♥

	elif msg.topic == "sensor":
		#Post the sensors

		paramUrl = 'http://iotplatform.ess/api/postsensor'
		paramJson = {'name': str(msg.payload.decode()), 'thingName': 'nodemcu'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(str(msg.payload.decode()))
		print(r) #Great success! ♥

	elif msg.topic == "lora_gateway":
		paramUrl = 'http://iotplatform.ess/api/postthinginfo'
		paramJson = {'value': str(msg.payload.decode()), 'thingName': 'esp32 lora gateway', 'designation': 'ESP32 LoRa'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(str(msg.payload.decode()))
		print(r) #Great success! ♥

	elif msg.topic == "lora_things":
		paramUrl = 'http://iotplatform.ess/api/postthinginfo'
		paramJson = {'value': str(msg.payload.decode()), 'thingName': 'esp32 lora', 'designation': 'ESP32 Lora Sensorial node'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(r)

	elif msg.topic == "lora_sensor":
		paramUrl = 'http://iotplatform.ess/api/postsensor'
		paramJson = {'name': str(msg.payload.decode()), 'thingName': 'esp32 lora'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(r)

	elif msg.topic == "lora_distance":
		paramUrl = 'http://iotplatform.ess/api/postvalue'
		paramJson = {'value': str(msg.payload.decode()), 'name': 'distance', 'sensorName': 'HC-SR04'}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(r)
    #print(msg.topic + " " + str(msg.payload.decode()))


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883, 60)  # localhost is the Raspberry Pi itself

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
