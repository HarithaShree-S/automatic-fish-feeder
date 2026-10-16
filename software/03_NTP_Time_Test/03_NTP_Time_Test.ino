#include <WiFi.h>
#include "time.h"

// Replace with your local Wi-Fi credentials
const char* ssid     = "SAINTGITS";
const char* password = "saintgitswifi";

// NTP Server settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800; // India Standard Time (IST) offset: +5:30 = 19800 seconds
const int   daylightOffset_sec = 0;

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("[ERROR] Failed to obtain time from NTP server");
    return;
  }
  // Print formatted time: HH:MM:SS
  Serial.println(&timeinfo, "[NTP TIME] %A, %B %d %Y %H:%M:%S");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println(" ESP32 Fish Feeder: 03_NTP_Time_Test     ");
  Serial.println("=========================================");

  // Connect to Wi-Fi network
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[STATUS] Wi-Fi Connected Successfully!");
  Serial.print("[STATUS] IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP time sync
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  printLocalTime();
  delay(5000); // Check and print time every 5 seconds
}