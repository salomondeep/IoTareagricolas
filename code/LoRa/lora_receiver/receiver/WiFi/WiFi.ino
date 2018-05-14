#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include "DHT.h"
#include <EEPROM.h>
#include <SPI.h>
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "labs" // enter your WiFi SSID
#define WLAN_PASS       "robot1cA!ESTG" // this is your WiFi password

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

/****************************** data stream publish/subscribe ***************************************/



/*************************** Sketch Code ************************************/
void setup() {
  Serial.begin(115200);
  delay(10);
  //pinMode(pot,INPUT); // potentiometer connected to A0 to obtain analog value to be streamed to Raspberry Pi (broker)

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

void loop() {
  delay(1250);
}


