#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#include <EEPROM.h>
#include <SPI.h>

#define dhtPIN D1
#define qrePIN A0 // define our potentiometer at pin0 which is the ADC for the ESP8266 board (the only ADC pin available)
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Alice" // enter your WiFi SSID
#define WLAN_PASS       "aliceinwonderland" // this is your WiFi password

/************************* Adafruit.io Setup *********************************/

#define MQTT_SERVER      "192.168.137.250" // change this to your Pi IP_address
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    "" // empty
#define MQTT_KEY         "" // empty

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

/****************************** data stream publish/subscribe ***************************************/

Adafruit_MQTT_Publish temp_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "temp"); // change the topic
Adafruit_MQTT_Publish humi_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "humi"); // change the topic
Adafruit_MQTT_Publish move_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "move"); // change the topic
Adafruit_MQTT_Publish thing_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "thingIp"); // change the topic
Adafruit_MQTT_Publish sensor_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "sensor"); // change the topic

//Adafruit_MQTT_Publish pi_led = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/led"); // ignore this for now


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

DHT dht;
int sensorValue = 0;
int sensorValueOn = 0;
float auxTemp = 0.0;
float auxHumid = 0.0;
boolean flag = true;
int tempoAtual = 0;
int tempoContadorDHT11 =  0;
int tempoContadorQRE =  0;
const int trezeSegundos = 13000;
const int dezSegundos = 10000;
int tempoContadorPublish = 0;

void setup() {
  Serial.begin(921600);
  delay(10);
  //pinMode(pot,INPUT); // potentiometer connected to A0 to obtain analog value to be streamed to Raspberry Pi (broker)
  Serial.println(F("Adafruit MQTT demo"));
  dht.setup(dhtPIN);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  String IP = WiFi.localIP().toString();
  char IP_char[15];
  IP.toCharArray(IP_char, 15);
  //sendThingInfo(WiFi.localIP().toString());
  //sendSensor("QRE");
  //sendSensor("DHT11");
  MQTT_connect();
  delay(500);
  thing_stream.publish(IP_char);
  delay(500);
  sensor_stream.publish("QRE");
  delay(500);
  sensor_stream.publish("DHT11");
  delay(500);
  move_stream.publish(0);
  delay(500);
  Serial.println("4 enviados");
  delay(500);
}

uint32_t x = 0;

void loop() {
  delay(25);
  tempoAtual = millis();

  if ((tempoAtual - tempoContadorDHT11) >= 5000)
  {
    readDHT();
    tempoContadorDHT11 = tempoAtual + 250;
  }

  if ((tempoAtual - tempoContadorQRE) >= 3999 /*&& sensorValueOn < 1024*/)
  {
    readQRE();
    tempoContadorQRE = tempoAtual + 250;
  }

  sendMovement();
}

void readDHT()
{
  float humidity = dht.getHumidity();
  delay(250);
  float temperature = dht.getTemperature();

  if (auxTemp != temperature && auxHumid != humidity)
  {
    sendToConsole(humidity, temperature);
    auxTemp = temperature;
    //delay(1000);
    temp_stream.publish(temperature);
    Serial.println("mandei temp");
    humi_stream.publish(humidity);
    Serial.println("Mandei humidade");
    //delay(1000);
    
  }
}

void readQRE()
{
  sensorValue = analogRead(qrePIN);
  //Serial.println(sensorValue);
  if (sensorValue < 1024)
  {

  } else
  {
    //delay(1000);
    move_stream.publish(0);
    Serial.println("publish 0 no movimento");
    //delay(1000);
  }
}

void sendMovement()
{
  sensorValueOn = analogRead(qrePIN);

  //Serial.println(sensorValue);
  if (sensorValueOn < 1024)
  {
    if ((tempoAtual - tempoContadorPublish) >= 3500)
    {
      //delay(1000);
      move_stream.publish(1);
      Serial.println("Mandei movimento!");
      //delay(1000);
      tempoContadorPublish = tempoAtual;
    }
  }
}








// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void sendToConsole(float humidity, float temperature)
{
  //Serial.print(dht.getStatusString());
  //Serial.print("\t");
  Serial.print(humidity);
  Serial.print("%");
  Serial.print(":");
  Serial.print(temperature);
  Serial.print("C");
  Serial.println();
  //temp_stream.publish(temperature);
}
