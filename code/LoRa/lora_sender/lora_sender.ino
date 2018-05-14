#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h"
#include "DHT.h"
#include <SPI.h>
#include <LoRa.h>
// #include "SSD1306.h"
#include<Arduino.h>
SSD1306 display(0x3c, 4, 15);

#define SS 18
#define RST 14
#define DI0 26
// #define BAND 429E6 //915E6
// #define BAND 434500000.00
#define BAND 434500000.00
#define spreadingFactor 9
// #define SignalBandwidth 62.5E3
#define SignalBandwidth 31.25E3
#define preambleLength 8
#define codingRateDenominator 8
#define trigPin 21
#define echoPin 13

int counter = 0;
float auxTemp = 0.0;

void setup() {
  pinMode(25,OUTPUT); //Send success, LED will bright 1 second
  
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);
  
  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

// Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(5,5,"LoRa Sender");
  display.display();
  
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);
  
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);

  LoRa.setPreambleLength(preambleLength);
  
  Serial.println("LoRa Initial OK!");
  display.drawString(5,20,"LoRa Initializing OK!");
  display.display();
  delay(1000);
  send_thingInfo();
  send_sensor();
}

void loop() {  
  send_distance();
  delay(200);
  digitalWrite(25, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000); // wait for a second
  digitalWrite(25, LOW); // turn the LED off by making the voltage LOW
  delay(1000); // wait for a second
  
 delay(5000);
}

void send_thingInfo()
{
    // send packet
  LoRa.beginPacket();
  LoRa.print("thingInfo:Lora Sender");
  LoRa.endPacket();
}

void send_sensor()
{
    // send packet
  LoRa.beginPacket();
  LoRa.print("sensorInfo:HC-SR04");
  LoRa.endPacket();
}

void send_distance()
{
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.println(distance);
  LoRa.beginPacket();
  LoRa.print("Distance:");
  LoRa.print(distance);
  LoRa.endPacket();
}

