/*
 * Project: ESP32 Automatic Fish Feeder
 * Module: 03_NTP_Time_Test
 * Description: Connects to Wi-Fi and synchronizes current time using NTP.
 */

#include <WiFi.h>
#include <time.h>

// Wi-Fi Credentials
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// NTP Server settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;    // Example: UTC+5:30 (India Standard Time = 5.5 * 3600 = 19800)
const int   daylightOffset_sec = 0;   // Set to 3600 if Daylight Saving applies

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time from NTP server");
    return;
  }
  Serial.println(&timeinfo, "Current Time: %Y-%m-%d %H:%M:%S (%A)");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("[ESP32 Fish Feeder] Step 03: NTP Time Test Started");

  // Connect to Wi-Fi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Init and get time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  delay(5000);
  printLocalTime();
}
