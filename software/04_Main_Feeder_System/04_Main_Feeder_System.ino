#include <WiFi.h>
#include <ESP32Servo.h>
#include "time.h"
// Wi-Fi credentials loaded from gitignored config.h (see config.example.h)
#if __has_include("config.h")
  #include "config.h"
#else
  #include "config.example.h"
#endif

// Hardware Pin Assignments
const int SERVO_PIN = 18;

// Servo Object
Servo feederServo;

// NTP Configuration (India Standard Time +5:30 = 19800 seconds)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// =========================================================================
// SET YOUR TEST TIME HERE (Change these to 1-2 mins ahead of your current time)
// =========================================================================
// Production Daily Feeding Schedule (24-Hour Format)
const int FEED_HOUR_1 = 8;   // Morning: 08:00 AM
const int FEED_MIN_1  = 0;

const int FEED_HOUR_2 = 18;  // Evening: 06:00 PM
const int FEED_MIN_2  = 0;

int lastFedMinute = -1; // Prevents duplicate triggers in the same minute

void dispenseFood() {
  Serial.println("\n[FEEDER ACTION] *** Feeding Time Reached! ***");
  
  // Rotate to 90 degrees to open slot
  feederServo.write(90);
  delay(1500); // Hold open for 1.5 seconds
  
  // Return to 0 degrees to close slot
  feederServo.write(0);
  Serial.println("[FEEDER ACTION] Dispensing complete. Closed slot.\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println("   ESP32 AUTOMATIC FISH FEEDER MAIN      ");
  Serial.println("=========================================");

  // Initialize Servo
  feederServo.attach(SERVO_PIN, 500, 2400);
  feederServo.write(0); // Start in sealed position

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[STATUS] Wi-Fi Connected!");
  Serial.print("[STATUS] IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("[ERROR] Failed to obtain NTP time");
    delay(2000);
    return;
  }

  int currentHour   = timeinfo.tm_hour;
  int currentMinute = timeinfo.tm_min;
  int currentSecond = timeinfo.tm_sec;

  // Print current status to Serial Monitor
  Serial.printf("[SYSTEM TIME] %02d:%02d:%02d\n", currentHour, currentMinute, currentSecond);

  // Check feeding schedule
  bool isFeedingTime1 = (currentHour == FEED_HOUR_1 && currentMinute == FEED_MIN_1);
  bool isFeedingTime2 = (currentHour == FEED_HOUR_2 && currentMinute == FEED_MIN_2);

  if ((isFeedingTime1 || isFeedingTime2) && lastFedMinute != currentMinute) {
    dispenseFood();
    lastFedMinute = currentMinute; // Lock feeding for this minute
  }

  // Reset lock when out of feeding minute
  if (!isFeedingTime1 && !isFeedingTime2) {
    lastFedMinute = -1;
  }

  delay(5000); // Check every 5 seconds
}