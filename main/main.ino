#include <Wire.h>
#include <AM2302-Sensor.h>

#define SLAVE_ADDRESS 8
#define WATER_SENSOR_PIN A0
#define DHT_DATA_PIN 2

AM2302::AM2302_Sensor dht(DHT_DATA_PIN);

int adc1 = 0;
int adc2 = 0;
float temperature = 0;
float humidity = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  dht.begin();
  delay(3000);
}

void loop() {
  // Read Sensor 1
  adc1 = analogRead(WATER_SENSOR_PIN);

  // Read Sensor 2 from I2C slave
  Wire.requestFrom(SLAVE_ADDRESS, 2);
  if (Wire.available() == 2) {
    byte high = Wire.read();
    byte low  = Wire.read();
    adc2 = word(high, low);
  }

  // Read temperature and humidity
  int status = dht.read();
  if (status == 0) {
    temperature = dht.get_Temperature();
    humidity = dht.get_Humidity();
  }

  // Send only clean data to ESP32
  Serial.print(temperature, 1);
  Serial.print(",");
  Serial.print(humidity, 1);
  Serial.print(",");
  Serial.print(adc1);
  Serial.print(",");
  Serial.println(adc2);

  delay(2000);
}