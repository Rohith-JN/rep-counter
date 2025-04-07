#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MPU6050 mpu;

float filteredPitch = 0;
const float alpha = 0.1;

float minPitchObserved = 999;
float maxPitchObserved = -999;

void setup() {
  Serial.begin(115200);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pitch Monitor");
  display.display();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  filteredPitch = alpha * pitch + (1 - alpha) * filteredPitch;

  // Track min/max observed pitch
  if (filteredPitch < minPitchObserved) minPitchObserved = filteredPitch;
  if (filteredPitch > maxPitchObserved) maxPitchObserved = filteredPitch;

  // Print to Serial
  Serial.print("Filtered Pitch: ");
  Serial.println(filteredPitch);

  // Update OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Pitch: ");
  display.println((int)filteredPitch);
  display.print("Min: ");
  display.println((int)minPitchObserved);
  display.print("Max: ");
  display.println((int)maxPitchObserved);
  display.display();

  delay(100);
}
