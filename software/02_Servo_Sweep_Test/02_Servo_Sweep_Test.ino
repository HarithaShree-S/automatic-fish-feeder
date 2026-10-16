#include <ESP32Servo.h>

Servo feederServo;
const int SERVO_PIN = 18;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=========================================");
  Serial.println(" ESP32 Fish Feeder: 02_Servo_Sweep_Test ");
  Serial.println("=========================================");

  // Attach servo to GPIO 18 with standard pulse widths
  feederServo.attach(SERVO_PIN, 500, 2400);
  feederServo.write(0); // Set initial position to 0 degrees
}

void loop() {
  Serial.println("[SERVO] Position: 0 degrees (Sealed/Closed)");
  feederServo.write(0);
  delay(2000);

  Serial.println("[SERVO] Position: 90 degrees (Dispensing)");
  feederServo.write(90);
  delay(2000);

  Serial.println("[SERVO] Position: 180 degrees (Fully Open)");
  feederServo.write(180);
  delay(2000);
}