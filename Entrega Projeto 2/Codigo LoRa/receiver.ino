#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <PubSubClient.h>
/************************* Adafruit.io Setup *********************************/

#define MQTT_SERVER      "192.168.137.250" // change this to your Pi IP_address
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    "" // empty
#define MQTT_KEY         "" // empty

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Alice" // enter your WiFi SSID
#define WLAN_PASS       "aliceinwonderland" // this is your WiFi password
SSD1306 display(0x3c, 4, 15);


/************ Global State (you don't need to change this!) ******************/
WiFiClient client;
PubSubClient sub_client(client);



#define SS 18
#define RST 14
#define DI0 26
// #define BAND 429E6

// LoRa Settings
#define BAND 434500000.00
#define spreadingFactor 9
// #define SignalBandwidth 62.5E3
#define SignalBandwidth 31.25E3

#define codingRateDenominator 8
int tempoAtual = millis();
int tempoContadorStatus =  0;

// ADC? Battery voltage
// const uint8_t vbatPin = 34;
// float VBAT;  // battery voltage from ESP32 ADC read

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

/****************************** data stream publish/subscribe ***************************************/
Adafruit_MQTT_Publish thing_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "lora_things"); // change the topic
Adafruit_MQTT_Publish sensor_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "lora_sensor"); // change the topic
Adafruit_MQTT_Publish value_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "lora_distance"); // change the topic
Adafruit_MQTT_Publish gateway_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "lora_gateway"); // change the topic
Adafruit_MQTT_Publish actuator_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "lora_actuator"); // change the topic
Adafruit_MQTT_Publish qualquercoisa_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "qualquercoisa"); // change the topic
Adafruit_MQTT_Publish lora_status_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "lora_status"); // change the topic

boolean lora_status = false;

void setup() {
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
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
  delay(1000);

  Serial.println("LoRa Receiver");
  display.drawString(5, 5, "LoRa Receiver");
  display.display();
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    display.drawString(5, 25, "Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");

  Serial.print("LoRa Frequency: ");
  Serial.println(BAND);

  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);

  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);

  display.drawString(5, 25, "LoRa Initializing OK!");
  display.display();

  sub_client.setServer(MQTT_SERVER, MQTT_SERVERPORT);
  sub_client.setCallback(callback);

  while (!sub_client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (sub_client.connect("ESP32Client", MQTT_USERNAME, MQTT_KEY )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(sub_client.state());
      delay(1000);
    }
  }

  String IP = WiFi.localIP().toString();
  char IP_char[15];
  IP.toCharArray(IP_char, 15);
  MQTT_connect();
  gateway_stream.publish(IP_char);
  Serial.println(IP);

  sub_client.subscribe("node");
}
Adafruit_MQTT_Subscribe *subscription;
void loop() {
  tempoAtual = millis();
  sub_client.loop();


  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packets
    Serial.print("Received packet. ");

    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(3, 0, "Received packet ");
    display.display();

    // read packet
    while (LoRa.available()) {
      String data = LoRa.readString();
      String part01 = getValue(data, ':', 0);
      //Serial.print(part01);

      if (part01 == "thingInfo")
      {
        String part02 = getValue(data, ':', 1);
        char thingInfo_char[15];
        part02.toCharArray(thingInfo_char, 15);
        thing_stream.publish(thingInfo_char);
      }

      if (part01 == "Distance")
      {
        String part02 = getValue(data, ':', 1);
        char distanceInfo_char[15];
        part02.toCharArray(distanceInfo_char, 15);
        value_stream.publish(distanceInfo_char);
      }

      if (part01 == "sensorInfo")
      {
        String part02 = getValue(data, ':', 1);
        char sensorInfo_char[15];
        part02.toCharArray(sensorInfo_char, 15);
        sensor_stream.publish(sensorInfo_char);
      }
      if (part01 == "actuatorInfo")
      {
        String part02 = getValue(data, ':', 1);
        char sensorInfo_char[15];
        part02.toCharArray(sensorInfo_char, 15);
        actuator_stream.publish(sensorInfo_char);
      }
      if (part01 == "status")
      {
        String part02 = getValue(data, ':', 1);
        char sensorInfo_char[15];
        part02.toCharArray(sensorInfo_char, 15);
        qualquercoisa_stream.publish(sensorInfo_char);
      }

      if (part01 == "thing_status") {
        String part02 = getValue(data, ':', 1);
        char lora_status_char[15];
        part02.toCharArray(lora_status_char, 15);
        lora_status_stream.publish(lora_status_char);
        Serial.println("Status received");

      }

      Serial.print(data);
      display.drawString(20, 20, data);
      display.display();
      lora_status = true;
    }

    // print RSSI of packet
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.print(" with SNR ");
    Serial.println(LoRa.packetSnr());
    // display.drawString(0, 45, "RSSI: ");
    // display.drawString(50, 45, (String)LoRa.packetRssi());

    display.drawString(0, 45, (String)LoRa.packetRssi() + "dB (" + (String)LoRa.packetSnr() + "dB)");

    display.display();
  } else
  {
    lora_status = false;
  }

  if ((tempoAtual - tempoContadorStatus) >= 60000)
  {
    if (!lora_status)
    {
      Serial.println("SEND THAT LORA IS DOWN");
      String part02 = "0";
      char lora_status_char[15];
      part02.toCharArray(lora_status_char, 15);
      lora_status_stream.publish(lora_status_char);

    }
    tempoContadorStatus = tempoAtual;
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


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Recebi sub");
  LoRa.beginPacket();
  LoRa.print("changeStatus");
  LoRa.endPacket();
}

void check_distance()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    while (LoRa.available()) {
      String data = LoRa.readString();
      String part01 = getValue(data, ':', 0);
      //Serial.print(part01);


    }
  }
}

