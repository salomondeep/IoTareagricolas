#include "DHT.h"
#include <EEPROM.h>
#include <SPI.h>

#define dhtPIN D1
#define qrePIN A0

DHT dht;
int sensorValue = 0;
float auxTemp = 0.0;
float auxHumid = 0.0;

void setup()
{
  Serial.begin(921600);

  dht.setup(dhtPIN);
}

void loop()
{
  readQRE();
  readDHT();
  delay(1250);
}

void readDHT()
{
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  if(auxTemp != temperature || auxHumid != humidity)
  {
      auxTemp = temperature;
      auxHumid = humidity;
      sendToConsole(humidity, temperature);
    }
  }

void sendToConsole(float humidity, float temperature)
{
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity);
  Serial.print("%");
  Serial.print("\t\t");
  Serial.print(temperature);
  Serial.print("C");
  Serial.println();
  }

void readQRE()
{
  sensorValue = analogRead(qrePIN);

  if(sensorValue < 1024)
  {
    Serial.println("Movement Detected!");
    }
  }

