#include <Arduino.h>
#include <WiFi.h>

// SmartRoll TEST 6A.1
// Wi-Fi diagnostic only.
// Existing 6A firmware is intentionally not modified.

const char* WIFI_SSID = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const uint32_t WIFI_RETRY_MS = 5000;
uint32_t lastAttempt = 0;

const char* wifiStatusName(wl_status_t s) {
  switch (s) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
    default: return "UNKNOWN";
  }
}

void printWiFiStatus() {
  wl_status_t s = WiFi.status();
  Serial.printf("Wi-Fi status: %d (%s)\n", (int)s, wifiStatusName(s));

  if (s == WL_CONNECTED) {
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("Subnet: %s\n", WiFi.subnetMask().toString().c_str());
    Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());
  }
}

void connectWiFi() {
  Serial.println("--------------------------------------------");
  Serial.println("Wi-Fi connection attempt");
  Serial.printf("SSID configured: %s\n", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t start = millis();
  wl_status_t previous = WL_IDLE_STATUS;

  while (millis() - start < 15000) {
    wl_status_t current = WiFi.status();
    if (current != previous) {
      Serial.printf("Wi-Fi status: %d (%s)\n", (int)current, wifiStatusName(current));
      previous = current;
    }

    if (current == WL_CONNECTED) {
      Serial.println("Wi-Fi: CONNECTED");
      printWiFiStatus();
      return;
    }
    delay(250);
  }

  Serial.println("Wi-Fi: connection timeout");
  printWiFiStatus();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 6A.1 - Wi-Fi diagnostics");
  Serial.println("============================================");
  Serial.println("This test does not use MQTT or RF.");

  WiFi.setSleep(false);
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastAttempt >= WIFI_RETRY_MS) {
      lastAttempt = millis();
      connectWiFi();
    }
  } else {
    static uint32_t lastReport = 0;
    if (millis() - lastReport >= 5000) {
      lastReport = millis();
      Serial.println("Wi-Fi: still connected");
      printWiFiStatus();
    }
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'p' || c == 'P') printWiFiStatus();
    if (c == 'r' || c == 'R') connectWiFi();
  }
}
