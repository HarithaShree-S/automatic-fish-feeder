/*
 * Project: ESP32 Automatic Fish Feeder
 * Module: 04_Main_Feeder_System
 * Description: Main integrated system with Wi-Fi, NTP time synchronization,
 *              scheduled feeding times, and servo-based dispenser control.
 * Requires: ESP32Servo library
 */

#include <WiFi.h>
#include <time.h>
#include <ESP32Servo.h>

// Wi-Fi Configuration
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// NTP Server Settings (UTC+5:30 default, adjust as needed)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Hardware Pins
const int SERVO_PIN = 18;
const int STATUS_LED = 2;

// Servo Angles & Timings
const int SERVO_CLOSED = 0;
const int SERVO_OPEN = 90;
const int DISPENSE_DURATION_MS = 1200;

// Scheduled Feeding Times (24-hour format: hour, minute)
struct FeedSchedule {
  int hour;
  int minute;
  bool fedToday;
};

FeedSchedule schedules[] = {
  { 8,  0, false }, // Morning feed at 08:00
  { 13, 0, false }, // Afternoon feed at 13:00
  { 20, 0, false }  // Evening feed at 20:00
};
const int NUM_SCHEDULES = sizeof(schedules) / sizeof(schedules[0]);

Servo feederServo;
int lastCheckedDay = -1;

void dispenseFood() {
  Serial.println("[FEEDER] Triggering food dispensing cycle...");
  digitalWrite(STATUS_LED, HIGH);

  feederServo.write(SERVO_OPEN);
  delay(DISPENSE_DURATION_MS);

  feederServo.write(SERVO_CLOSED);
  delay(300);

  digitalWrite(STATUS_LED, LOW);
  Serial.println("[FEEDER] Feeding cycle completed.");
}

void checkSchedules(struct tm* timeinfo) {
  // Reset daily feeding flags at midnight
  if (timeinfo->tm_mday != lastCheckedDay) {
    lastCheckedDay = timeinfo->tm_mday;
    for (int i = 0; i < NUM_SCHEDULES; i++) {
      schedules[i].fedToday = false;
    }
    Serial.println("[FEEDER] New day started. Daily schedules reset.");
  }

  for (int i = 0; i < NUM_SCHEDULES; i++) {
    if (!schedules[i].fedToday &&
        timeinfo->tm_hour == schedules[i].hour &&
        timeinfo->tm_min == schedules[i].minute) {
      Serial.printf("[FEEDER] Feeding schedule match (%02d:%02d)!\n",
                    schedules[i].hour, schedules[i].minute);
      dispenseFood();
      schedules[i].fedToday = true;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(STATUS_LED, OUTPUT);
  delay(1000);
  Serial.println("[ESP32 Fish Feeder] Starting Main Feeder System...");

  // Initialize Servo
  ESP32PWM::allocateTimer(0);
  feederServo.setPeriodHertz(50);
  feederServo.attach(SERVO_PIN, 500, 2400);
  feederServo.write(SERVO_CLOSED);

  // Connect to Wi-Fi
  Serial.printf("Connecting to Wi-Fi '%s' ", ssid);
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected. IP: ");
    Serial.println(WiFi.localIP());
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  } else {
    Serial.println("\nWiFi connection failed! Proceeding in offline mode.");
  }
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    checkSchedules(&timeinfo);
  } else {
    Serial.println("[WARNING] Waiting for NTP time synchronization...");
  }
  delay(10000); // Check every 10 seconds
}
