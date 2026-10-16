#include <ESP32Servo.h>

Servo feederServo;
const int SERVO_PIN = 18;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=========================================");
  Serial.println(" ESP32 Fish Feeder: 02_Servo_Sweep_Test ");
  Serial.println("=========================================");

  // Attach servo to GPIO 18 with standard 500us to 2400us pulse bounds
  feederServo.attach(SERVO_PIN, 500, 2400);

  // Set initial position to 0 degrees (Sealed state)
  feederServo.write(0);
  delay(1000);
}

void loop() {
  // Move to 0 degrees (Closed / Idle position)
  Serial.println("[SERVO] Position: 0 degrees (Sealed)");
  feederServo.write(0);
  delay(2000);

  // Move to 90 degrees (Dispensing rotation)
  Serial.println("[SERVO] Position: 90 degrees (Dispensing)");
  feederServo.write(90);
  delay(2000);

  // Move to 180 degrees (Full open rotation)
  Serial.println("[SERVO] Position: 180 degrees (Full Open)");
  feederServo.write(180);
  delay(2000);
}