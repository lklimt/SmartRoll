#include <Arduino.h>
#include <WiFi.h>

const char* WIFI_SSID = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 6A.2 - Wi-Fi scan diagnostic");
  Serial.println("============================================");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(500);

  Serial.println("Scanning 2.4 GHz Wi-Fi networks...");
  int n = WiFi.scanNetworks(false, true);

  if (n < 0) {
    Serial.printf("Scan failed: %d\n", n);
    return;
  }

  Serial.printf("Networks found: %d\n\n", n);
  bool found = false;

  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);
    wifi_auth_mode_t enc = WiFi.encryptionType(i);
    int channel = WiFi.channel(i);

    Serial.printf("%2d: SSID=\"%s\"  CH=%d  RSSI=%d dBm  AUTH=%d\n",
                  i + 1, ssid.c_str(), channel, rssi, (int)enc);

    if (ssid == WIFI_SSID) found = true;
  }

  Serial.println();
  Serial.printf("Target SSID: \"%s\"\n", WIFI_SSID);
  Serial.printf("Target SSID visible: %s\n", found ? "YES" : "NO");

  if (!found) {
    Serial.println("RESULT: ESP32 cannot see the configured SSID.");
    Serial.println("Check that the IoT WLAN is 2.4 GHz and is being advertised.");
    return;
  }

  Serial.println("Attempting connection...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t start = millis();
  wl_status_t last = WL_IDLE_STATUS;
  while (millis() - start < 20000) {
    wl_status_t s = WiFi.status();
    if (s != last) {
      Serial.printf("Wi-Fi status: %d\n", (int)s);
      last = s;
    }
    if (s == WL_CONNECTED) {
      Serial.println("RESULT: CONNECTED");
      Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
      Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
      Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
      return;
    }
    delay(250);
  }

  Serial.println("RESULT: CONNECTION FAILED");
  Serial.printf("Final status: %d\n", (int)WiFi.status());
}

void loop() {
}
