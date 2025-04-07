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
unsigned long lastRepTime = 0;
const unsigned long minRepInterval = 800; // ms

float filteredPitch = 0;
const float alpha = 0.1;

const float minPitch = -36;
const float maxPitch = 85;

// Threshold to detect motion
const float gyroThreshold = 2.0;

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
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Reps: 0");
  display.display();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;
  float gx = g.gyro.x;
  float gy = g.gyro.y;
  float gz = g.gyro.z;

  float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  filteredPitch = alpha * pitch + (1 - alpha) * filteredPitch;

  float gyroMagnitude = sqrt(gx * gx + gy * gy + gz * gz) * 180.0 / PI;

  unsigned long currentTime = millis();

  if (gyroMagnitude > gyroThreshold) {
    if (!isCurlingUp && filteredPitch > 45) {
      isCurlingUp = true;
      Serial.println("Up motion detected");
    }

    if (isCurlingUp && filteredPitch < 20) {
      isCurlingUp = false;

      if (currentTime - lastRepTime > minRepInterval) {
        lastRepTime = currentTime;
        repCount++;
        Serial.print("Valid Rep! Total: ");
        Serial.println(repCount);
      } else {
        Serial.println("Too fast, ignored");
      }
    }
  }

  display.clearDisplay();

  // Draw progress bar at top
  int barHeight = 6;
  int barY = 0;
  display.drawRect(0, barY, SCREEN_WIDTH, barHeight, SSD1306_WHITE);

  float progress = (filteredPitch - minPitch) / (maxPitch - minPitch);
  progress = constrain(progress, 0.0, 1.0);
  int barWidth = progress * (SCREEN_WIDTH - 2);
  if (barWidth > 0) {
    display.fillRect(1, barY + 1, barWidth, barHeight - 2, SSD1306_WHITE);
  }

  // Draw rep count at bottom
  display.setTextSize(2);
  display.setCursor(0, SCREEN_HEIGHT - 16);  // 16px tall text at bottom
  display.print("Reps: ");
  display.println(repCount);

  display.display();
  delay(40);
}
