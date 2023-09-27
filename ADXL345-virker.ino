#include "application.h"
#include <C:\Particle\EksempelADXL345test\lib\adxl345\src\ADXL345.h>

ADXL345 accelerometer;

void setup() {
  Serial.begin(9600);
  accelerometer.powerOn();
  accelerometer.setRangeSetting(16); // 16 g-range
}

void loop() {
  int x, y, z;
  accelerometer.readAccel(&x, &y, &z);
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.println(z);
  delay(1000);
}
