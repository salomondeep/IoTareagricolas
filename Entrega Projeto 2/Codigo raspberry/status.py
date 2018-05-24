import subprocess
import time
import os
import requests

while True:
	#RASPBERRY
	address_rpi = "192.168.137.250"
	res = subprocess.call(['ping', '-c', '3', address_rpi])
	if res == 0:
		#POST TO API
		print("pinged rpi")
		paramUrl = 'http://iotplatform.ess/api/poststatus'
		paramJson = {'ip': address_rpi, 'status': 1}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
	elif res != 0:
		print("No ping")
		paramUrl = 'http://iotplatform.ess/api/poststatus'
		paramJson = {'ip': address_rpi, 'status': 0}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)

	#NODEMCU
	address_node = "192.168.1.110"
	res = subprocess.call(['ping', '-c', '3', address_node])
	if res == 0:
		#POST TO API
		print("pinged node")
		paramUrl = 'http://iotplatform.ess/api/poststatus'
		paramJson = {'ip': address_node, 'status': 1}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(r)
	elif res != 0:
		print("No ping")
		paramUrl = 'http://iotplatform.ess/api/poststatus'
		paramJson = {'ip': address_node, 'status': 0}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)

	#LORA GATEWAY
	address_lora = "192.168.137.161"
	res = subprocess.call(['ping', '-c', '3', address_lora])
	if res == 0:
		#POST TO API
		print("pinged lora")
		paramUrl = 'http://iotplatform.ess/api/poststatus'
		paramJson = {'ip': address_lora, 'status': 1}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)
		print(r)
	elif res != 0:
		print("No ping")
		paramUrl = 'http://iotplatform.ess/api/poststatus'
		paramJson = {'ip': address_lora, 'status': 0}
		paramHeaders = {'content-type': 'application/json'}
		r = requests.post(paramUrl, json=paramJson, headers=paramHeaders)

	time.sleep(10)
