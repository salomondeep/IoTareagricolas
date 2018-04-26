/***************************************************
  Adafruit MQTT Library ESP8266 Example
  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#include <EEPROM.h>
#include <SPI.h>

#define dhtPIN D1
#define qrePIN A0 // define our potentiometer at pin0 which is the ADC for the ESP8266 board (the only ADC pin available)
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "labs" // enter your WiFi SSID
#define WLAN_PASS       "robot1cA!ESTG" // this is your WiFi password

/************************* Adafruit.io Setup *********************************/

#define MQTT_SERVER      "10.20.229.31" // change this to your Pi IP_address
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

//Adafruit_MQTT_Publish pi_led = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/pi/led"); // ignore this for now


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

DHT dht;
int sensorValue = 0;
float auxTemp = 0.0;
float auxHumid = 0.0;

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


  
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  
  MQTT_connect();

  readQRE();
  readDHT();
  delay(1250);
  //stream.publish(); // publish to Raspberry Pi under topic "/esp/pot"


  // ignore these for now
//  if(pot_value > 500)
//    pi_led.publish("ledon");
//  else if(pot_value < 500)
//    pi_led.publish("ledoff");
    

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

void readDHT()
{
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();


  
  if(auxTemp != temperature)
  {
      auxTemp = temperature;
      temp_stream.publish(temperature);
      sendToConsole(humidity, temperature);
  }

    if(auxHumid != humidity)
    {
      auxHumid = humidity;
      humi_stream.publish(humidity);
    }
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
  //temp_stream.publish(temperature); // publish to Raspberry Pi under topic "/esp/pot"
  }

void readQRE()
{
  sensorValue = analogRead(qrePIN);
  //Serial.println(sensorValue);
  if(sensorValue < 1024)
  {
    Serial.println("Movement Detected!");
    move_stream.publish("movement!");
    }
  }
