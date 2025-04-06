#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MPU6050 mpu;

int repCount = 0;
bool isCurlingUp = false;

void setup() {
  Serial.begin(115200);

  // Init MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Reps: 0");
  display.display();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  // Calculate pitch angle (in degrees)
  float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

  Serial.print("Pitch: ");
  Serial.println(pitch);

  // Detect a rep:
  // Going up
  if (!isCurlingUp && pitch > 50) {
    isCurlingUp = true;
  }

  // Coming back down
  if (isCurlingUp && pitch < 20) {
    isCurlingUp = false;
    repCount++;

    // Update display
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Reps: ");
    display.println(repCount);
    display.display();

    Serial.print("Rep Counted! Total: ");
    Serial.println(repCount);
  }

  delay(50); // smoothing
}   