#include <WiFi.h>
#include "time.h"

// Wi-Fi credentials loaded from gitignored config.h (see config.example.h)
#if __has_include("config.h")
  #include "config.h"
#else
  #include "config.example.h"
#endif

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// NTP Server settings (Configured for India Standard Time +5:30)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800; // +5 hours 30 mins = 19800 seconds
const int   daylightOffset_sec = 0;

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("[ERROR] Failed to obtain time from NTP server");
    return;
  }
  // Format and print: Day, Month Date Year HH:MM:SS
  Serial.println(&timeinfo, "[NTP TIME] %A, %B %d %Y %H:%M:%S");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println(" ESP32 Fish Feeder: 03_NTP_Time_Test     ");
  Serial.println("=========================================");

  // Connect to phone hotspot
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

  // Synchronize time from NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  printLocalTime();
  delay(5000); // Fetch and display live time every 5 seconds
}