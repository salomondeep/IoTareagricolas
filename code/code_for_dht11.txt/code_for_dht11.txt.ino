#include <DHT11.h>
int dhtPin = D1;

DHT11 dht11(dhtPin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(921600);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int readData = dht.read11(dhtPin); // Reads the data from the sensor
  float t = dht.readTemperature(); // Gets the values of the temperature
  //float h = DHT.readHumidity(); // Gets the values of the humidity
  
  // Printing the results on the serial monitor
  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.print(" *C ");
  //Serial.print("    Humidity = ");
  //Serial.print(h);
  //Serial.println(" % ");
  
  delay(2000); // Delays 2 secods, as the DHT22 sampling rate is 0.5Hz
}
