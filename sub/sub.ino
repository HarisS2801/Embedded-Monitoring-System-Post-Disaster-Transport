#include <Wire.h>

#define SLAVE_ADDRESS 8
#define WATER_SENSOR_PIN A0

volatile int adcValue2 = 0;

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendADC);
}

void loop() {
  adcValue2 = analogRead(WATER_SENSOR_PIN);
  delay(50);
}

void sendADC() {
  Wire.write((byte)(adcValue2 >> 8));
  Wire.write((byte)(adcValue2 & 0xFF));
}