import serial
import requests
import os
from datetime import datetime
import socket
import time

os.system("clear")

#SERIAL COMMUNICATION
try:
    ser = serial.Serial('/dev/ttyACM0', 9600)
    s = [0]
    print("deu 0")
except:
    print("nao deu")

try:
    ser = serial.Serial('/dev/ttyACM1', 9600)
    s = [0]
    print("deu 1")
except:
    print("nao deu")

    try:
        ser = serial.Serial('/dev/ttyACM2', 9600)
        s = [0]
        print("deu 2")
    except:
        print("nao deu")

try:
    ser = serial.Serial('/dev/ttyACM3', 9600)
    s = [0]
    print("deu 3")
except:
    print("nao deu")

#ser = serial.Serial('/dev/ttyACM0', 9600)
#print(ser)

# GLOBAL VARS
url = "http://iotplatform.ess/api/"
flagReadCardKey = 0
flagOpenOrClose = 0
cardKey = ""

# STRINGS TO COMPARE
masterCardNotDefined = "No Master Card Defined"
masterCardDefined = "Master Card's UID"
masterProgramMode = "Hello Master - Entered Program Mode"
masterNormalMode = "Exiting Program Mode"
moveDetection = "Motion detected"
moveDetectionEnded = "Motion ended"
scannedPICC = "Scanned PICC's UID:"
slavePermitedCard = "Welcome, You shall pass"

# GET IP ADDRESS
gw = os.popen("ip -4 route show default").read().split()
socketInfo = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
socketInfo.connect((gw[2], 0))
ipaddr = socketInfo.getsockname()[0]
#gateway = gw[2]
#host = socket.gethostname()
#print ("IP:", ipaddr, " GW:", gateway, " Host:", host)

def postToIoTPlatform(messageToPrint, apiRoute, jsonParam):
    print(messageToPrint)
    paramUrl = url + apiRoute
    paramJSON = jsonParam
    paramHeaders = {'content-type': 'application/json'}
    r = requests.post(paramUrl, json=paramJSON, headers=paramHeaders)
    print(r.text)
    return 0

message = "###### SEND THING INFO / IP / NAME #####"
thingRoute = "postthinginfo"
paramJSON = {'value': ipaddr, 'thingName': "Raspberry PI 3", "designation": "It Room"}
postToIoTPlatform(message, thingRoute, paramJSON)

message = "###### SEND THING INFO / IP / NAME #####"
thingRoute = "postthinginfo"
paramJSON = {'value': "SERIAL", 'thingName': "Arduino MEGA 2560", "designation": "It Room"}
postToIoTPlatform(message, thingRoute, paramJSON)

message = "###### SEND SENSOR INFO / NAME #####"
thingRoute = "postsensor"
paramJSON = {'name': "PIR", 'thingName': "Arduino MEGA 2560"}
postToIoTPlatform(message, thingRoute, paramJSON)

message = "###### SEND MEGA ACTIVE #####"
thingRoute = "poststatus"
paramJSON = {'ip': "SERIAL", 'status': 1}
postToIoTPlatform(message, thingRoute, paramJSON)


x = 1
z = 0
y = 0
if x == 1:
    while True:
        try:
            #ser = serial.Serial('/dev/ttyACM0', 9600)
            #s = [0]
            read_serial = ser.readline()
            print(read_serial.decode().strip('\r\n'))
            read_serial = read_serial.decode().strip('\r\n')
            z = 1
            if y == 1:
                message = "###### SEND MEGA ACTIVE #####"
                thingRoute = "poststatus"
                paramJSON = {'ip': "SERIAL", 'status': 1}
                postToIoTPlatform(message, thingRoute, paramJSON)
                y = 0
        except:
            if y == 0:
                print("########## EXCEPTION #############")
                message = "###### SEND MEGA INACTIVE #####"
                thingRoute = "poststatus"
                paramJSON = {'ip': "SERIAL", 'status': 0}
                postToIoTPlatform(message, thingRoute, paramJSON)
                y = 1

            try:
                ser = serial.Serial('/dev/ttyACM0', 9600)
                s = [0]
                print("deu 0")
            except:
                print("nao deu 0")

            try:
                ser = serial.Serial('/dev/ttyACM1', 9600)
                s = [0]
                print("deu 1")
            except:
                print("nao deu 1")

                try:
                    ser = serial.Serial('/dev/ttyACM2', 9600)
                    s = [0]
                    print("deu 2")
                except:
                    print("nao deu 2")

            try:
                ser = serial.Serial('/dev/ttyACM3', 9600)
                s = [0]
                print("deu 3")
            except:
                print("nao deu 3")
            z = 0

        if z == 1:
            # s[0] = str(int (ser.readline(),16))
            if flagReadCardKey == 1:
                cardKey = read_serial
                print('#######   ' + cardKey + '   #####')
                flagReadCardKey = 0

            if read_serial == scannedPICC:
                flagReadCardKey = 1

            if read_serial == masterCardNotDefined:
                print('####### no master defined #####')
                # send status by post

            if read_serial == masterCardDefined:
                print('####### master card is defined #####')
                # send status by post

            if read_serial == masterProgramMode:
                message = '####### program mode #####'
                thingRoute = 'doorstatusopen'
                paramJSON = {'status': '1', 'key': cardKey}
                postToIoTPlatform(message, thingRoute, paramJSON)

            if read_serial == masterNormalMode:
                message = '####### normal mode #####'
                thingRoute = 'doorstatusclose'
                paramJSON = {'status': '0', 'key': cardKey}
                postToIoTPlatform(message, thingRoute, paramJSON)

            if read_serial == slavePermitedCard:
                if flagOpenOrClose == 0:
                    flagOpenOrClose = 1
                else:
                    flagOpenOrClose = 0

                message = '####### slave permited - you can pass #####'
                thingRoute = 'doorstatusclose'
                paramJSON = {'status': flagOpenOrClose, 'key': cardKey}
                postToIoTPlatform(message, thingRoute, paramJSON)

            if read_serial == moveDetection:
                message = '####### move detected #####'
                thingRoute = 'postvalue'
                paramJSON = {'name': "PIR Movement", 'value': 1, 'sensorName': 'PIR'}
                postToIoTPlatform(message, thingRoute, paramJSON)


            if read_serial == moveDetectionEnded:
                message = '####### move detected end #####'
                thingRoute = 'postvalue'
                paramJSON = {'name': "PIR Movement", 'value': 0, 'sensorName': 'PIR'}
                postToIoTPlatform(message, thingRoute, paramJSON)


