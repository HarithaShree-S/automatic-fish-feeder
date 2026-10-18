#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "time.h"
#include "web_page.h"

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

// Web Server on port 80
WebServer server(80);

// NTP Configuration (India Standard Time +5:30 = 19800 seconds)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// Daily Feeding Schedule (24-Hour Format, mutable via Web Dashboard)
int feedHour1 = 8;   // Morning: 08:00 AM default
int feedMin1  = 0;

int feedHour2 = 18;  // Evening: 06:00 PM default
int feedMin2  = 0;

int lastFedMinute = -1; // Prevents duplicate triggers in the same minute
char lastFedTimestamp[32] = "Not yet fed today";
unsigned long lastCheckMillis = 0;

// =========================================================================
// SERVO DISPENSE FUNCTION
// =========================================================================
void dispenseFood(const char* triggerSource = "Scheduled") {
  Serial.printf("\n[FEEDER ACTION] *** Feeding Triggered (%s)! ***\n", triggerSource);
  
  // Rotate to 90 degrees to open dispenser slot
  feederServo.write(90);
  delay(1500); // Hold open for 1.5 seconds for food pellets to drop
  
  // Return to 0 degrees to seal slot
  feederServo.write(0);
  Serial.println("[FEEDER ACTION] Dispensing complete. Closed slot.\n");

  // Update last fed timestamp
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    strftime(lastFedTimestamp, sizeof(lastFedTimestamp), "%H:%M:%S (%b %d)", &timeinfo);
  } else {
    snprintf(lastFedTimestamp, sizeof(lastFedTimestamp), "Just now (manual)");
  }
}

// =========================================================================
// WEB SERVER HANDLERS
// =========================================================================

// CORS helper headers to enable browser interaction from any origin
void setCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// Serve Ocean-Themed Dashboard on GET /
void handleRoot() {
  setCorsHeaders();
  server.send_P(200, "text/html", INDEX_HTML);
}

// Trigger Manual Feeding on /feed (GET or POST)
void handleFeed() {
  setCorsHeaders();
  dispenseFood("Web Dashboard");

  String json = "{\"status\":\"success\",\"message\":\"Food dispensed successfully!\",\"lastFed\":\"" + String(lastFedTimestamp) + "\"}";
  server.send(200, "application/json", json);
}

// Update Schedule on /set-schedule (supports GET params, POST form, or JSON body)
void handleSetSchedule() {
  setCorsHeaders();

  bool updated = false;

  // 1. Direct hour and minute parameters (e.g. ?m_hour=8&m_min=30&e_hour=18&e_min=0)
  if (server.hasArg("m_hour") && server.hasArg("m_min")) {
    feedHour1 = server.arg("m_hour").toInt();
    feedMin1  = server.arg("m_min").toInt();
    updated = true;
  }
  if (server.hasArg("e_hour") && server.hasArg("e_min")) {
    feedHour2 = server.arg("e_hour").toInt();
    feedMin2  = server.arg("e_min").toInt();
    updated = true;
  }

  // 2. String time parameters (e.g. ?morning=08:30&evening=18:00)
  if (server.hasArg("morning")) {
    String m = server.arg("morning");
    int colon = m.indexOf(':');
    if (colon > 0) {
      feedHour1 = m.substring(0, colon).toInt();
      feedMin1  = m.substring(colon + 1).toInt();
      updated = true;
    }
  }
  if (server.hasArg("evening")) {
    String e = server.arg("evening");
    int colon = e.indexOf(':');
    if (colon > 0) {
      feedHour2 = e.substring(0, colon).toInt();
      feedMin2  = e.substring(colon + 1).toInt();
      updated = true;
    }
  }

  // 3. Raw JSON payload in request body
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    int mIdx = body.indexOf("\"morning\"");
    if (mIdx != -1) {
      int valStart = body.indexOf("\"", mIdx + 9);
      if (valStart != -1) {
        valStart++;
        int valEnd = body.indexOf("\"", valStart);
        if (valEnd != -1) {
          String m = body.substring(valStart, valEnd);
          int colon = m.indexOf(':');
          if (colon > 0) {
            feedHour1 = m.substring(0, colon).toInt();
            feedMin1  = m.substring(colon + 1).toInt();
            updated = true;
          }
        }
      }
    }
    int eIdx = body.indexOf("\"evening\"");
    if (eIdx != -1) {
      int valStart = body.indexOf("\"", eIdx + 9);
      if (valStart != -1) {
        valStart++;
        int valEnd = body.indexOf("\"", valStart);
        if (valEnd != -1) {
          String e = body.substring(valStart, valEnd);
          int colon = e.indexOf(':');
          if (colon > 0) {
            feedHour2 = e.substring(0, colon).toInt();
            feedMin2  = e.substring(colon + 1).toInt();
            updated = true;
          }
        }
      }
    }
  }

  char mBuffer[8], eBuffer[8];
  snprintf(mBuffer, sizeof(mBuffer), "%02d:%02d", feedHour1, feedMin1);
  snprintf(eBuffer, sizeof(eBuffer), "%02d:%02d", feedHour2, feedMin2);

  Serial.printf("[SCHEDULE UPDATE] Morning: %s | Evening: %s\n", mBuffer, eBuffer);

  String response = "{\"status\":\"success\",\"message\":\"Schedule updated successfully\",\"morning\":\"" + String(mBuffer) + "\",\"evening\":\"" + String(eBuffer) + "\"}";
  server.send(200, "application/json", response);
}

// Return current status, time, and schedule on /status
void handleStatus() {
  setCorsHeaders();

  char mBuffer[8], eBuffer[8], timeBuffer[16];
  snprintf(mBuffer, sizeof(mBuffer), "%02d:%02d", feedHour1, feedMin1);
  snprintf(eBuffer, sizeof(eBuffer), "%02d:%02d", feedHour2, feedMin2);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeinfo);
  } else {
    snprintf(timeBuffer, sizeof(timeBuffer), "--:--:--");
  }

  String json = "{";
  json += "\"status\":\"online\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"currentTime\":\"" + String(timeBuffer) + "\",";
  json += "\"morning\":\"" + String(mBuffer) + "\",";
  json += "\"evening\":\"" + String(eBuffer) + "\",";
  json += "\"lastFed\":\"" + String(lastFedTimestamp) + "\",";
  json += "\"uptime\":" + String(millis() / 1000);
  json += "}";

  server.send(200, "application/json", json);
}

// CORS Preflight handler
void handleOptions() {
  setCorsHeaders();
  server.send(204);
}

// =========================================================================
// SETUP
// =========================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println("   ESP32 AUTOMATIC FISH FEEDER SYSTEM    ");
  Serial.println("=========================================");

  // Initialize Servo
  feederServo.attach(SERVO_PIN, 500, 2400);
  feederServo.write(0); // Start in sealed closed position

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

  // Configure NTP (India Standard Time +5:30)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("[STATUS] NTP Time service configured");

  // Web Server Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/feed", HTTP_POST, handleFeed);
  server.on("/feed", HTTP_GET, handleFeed);
  server.on("/feed", HTTP_OPTIONS, handleOptions);
  server.on("/set-schedule", HTTP_POST, handleSetSchedule);
  server.on("/set-schedule", HTTP_GET, handleSetSchedule);
  server.on("/set-schedule", HTTP_OPTIONS, handleOptions);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/status", HTTP_OPTIONS, handleOptions);

  server.begin();
  Serial.println("[STATUS] Web Server listening on port 80");
  Serial.print("[STATUS] Access Web Dashboard at: http://");
  Serial.println(WiFi.localIP());
}

// =========================================================================
// SCHEDULE CHECKER
// =========================================================================
void checkSchedule() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("[ERROR] Failed to obtain NTP time");
    return;
  }

  int currentHour   = timeinfo.tm_hour;
  int currentMinute = timeinfo.tm_min;
  int currentSecond = timeinfo.tm_sec;

  // Print time ticker to Serial Monitor every 10 seconds
  if (currentSecond % 10 == 0) {
    Serial.printf("[SYSTEM TIME] %02d:%02d:%02d (Scheduled: %02d:%02d & %02d:%02d)\n",
                  currentHour, currentMinute, currentSecond,
                  feedHour1, feedMin1, feedHour2, feedMin2);
  }

  // Check feeding schedule match
  bool isFeedingTime1 = (currentHour == feedHour1 && currentMinute == feedMin1);
  bool isFeedingTime2 = (currentHour == feedHour2 && currentMinute == feedMin2);

  if ((isFeedingTime1 || isFeedingTime2) && lastFedMinute != currentMinute) {
    dispenseFood(isFeedingTime1 ? "Morning Schedule" : "Evening Schedule");
    lastFedMinute = currentMinute; // Lock feeding for this minute
  }

  // Reset lock when out of feeding minute
  if (!isFeedingTime1 && !isFeedingTime2) {
    lastFedMinute = -1;
  }
}

// =========================================================================
// MAIN LOOP
// =========================================================================
void loop() {
  // Handle incoming web client requests immediately and non-blockingly
  server.handleClient();

  // Periodically check feeding schedule every 1 second
  unsigned long currentMillis = millis();
  if (currentMillis - lastCheckMillis >= 1000) {
    lastCheckMillis = currentMillis;
    checkSchedule();
  }
}