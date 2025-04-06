#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nI2C Scanner Running...");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found\n");
  else Serial.println("Scan complete.\n");

  delay(5000);
}
