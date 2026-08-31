/*
  SmartRoll - TEST 6A.4
  Wi-Fi + MQTT isolation test

  Purpose:
    Verify that ESP32 can maintain a stable Wi-Fi connection and connect
    to the SmartRoll MQTT broker before Hall, RF and sensor subsystems are
    added again.

  PIR is NOT part of SmartRoll.

  Serial Monitor: 115200 Bd

  Commands:
    p = print status
    r = restart Wi-Fi connection
    m = force MQTT reconnect
    h = help

  Do NOT store real credentials in GitHub.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ---------- LOCAL CONFIGURATION ----------
const char* WIFI_SSID     = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_HOST     = "YOUR_MQTT_BROKER_IP";
const uint16_t MQTT_PORT  = 1883;
const char* MQTT_USER     = "YOUR_MQTT_USER";
const char* MQTT_PASSWORD = "YOUR_MQTT_PASSWORD";

const char* DEVICE_ID = "smartroll_01_test6a4";

const uint32_t WIFI_RETRY_MS = 5000;
const uint32_t MQTT_RETRY_MS = 5000;
const uint32_t STATUS_MS = 3000;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

uint32_t lastWiFiAttempt = 0;
uint32_t lastMQTTAttempt = 0;
uint32_t lastStatus = 0;
uint32_t mqttMessages = 0;

String topicState;
String topicCommand;
String topicAvailability;

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
  Serial.printf("Wi-Fi: %d (%s)\n", (int)s, wifiStatusName(s));

  if (s == WL_CONNECTED) {
    Serial.printf("  SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("  IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("  Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI());
    Serial.printf("  MAC: %s\n", WiFi.macAddress().c_str());
  }
}

void connectWiFi() {
  Serial.println("Wi-Fi: connection attempt");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  mqttMessages++;
  Serial.printf("MQTT RX [%s]: ", topic);
  for (unsigned int i = 0; i < length; i++) Serial.print((char)payload[i]);
  Serial.println();
}

void connectMQTT() {
  if (WiFi.status() != WL_CONNECTED) return;

  Serial.println("MQTT: connection attempt");

  String clientId = String(DEVICE_ID) + "_" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool ok;
  if (strlen(MQTT_USER) == 0) {
    ok = mqtt.connect(clientId.c_str(),
                      topicAvailability.c_str(), 0, true, "offline");
  } else {
    ok = mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD,
                      topicAvailability.c_str(), 0, true, "offline");
  }

  if (ok) {
    Serial.println("MQTT: CONNECTED");
    mqtt.publish(topicAvailability.c_str(), "online", true);
    mqtt.subscribe(topicCommand.c_str());
    mqtt.publish(topicState.c_str(), "TEST6A4_ONLINE", true);
  } else {
    Serial.printf("MQTT: FAILED state=%d\n", mqtt.state());
  }
}

void printStatus() {
  Serial.println("--------------------------------------------");
  printWiFiStatus();
  Serial.printf("MQTT: %s", mqtt.connected() ? "CONNECTED" : "OFFLINE");
  if (mqtt.connected()) Serial.printf("  messages=%lu", (unsigned long)mqttMessages);
  Serial.println();
  Serial.println("--------------------------------------------");
}

void printHelp() {
  Serial.println("Commands: p=status, r=Wi-Fi reconnect, m=MQTT reconnect, h=help");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 6A.4");
  Serial.println("Wi-Fi + MQTT isolation test");
  Serial.println("============================================");

  topicState = String("smartroll/") + DEVICE_ID + "/state";
  topicCommand = String("smartroll/") + DEVICE_ID + "/command";
  topicAvailability = String("smartroll/") + DEVICE_ID + "/availability";

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  // Use the connection sequence proven by the Wi-Fi diagnostic test.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(300);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Wi-Fi: connecting...");
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();

  printWiFiStatus();

  // First MQTT attempt is made immediately if Wi-Fi is already connected.
  connectMQTT();
  printHelp();
}

void loop() {
  uint32_t now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWiFiAttempt >= WIFI_RETRY_MS) {
      lastWiFiAttempt = now;
      connectWiFi();
    }
  }

  if (WiFi.status() == WL_CONNECTED && !mqtt.connected()) {
    if (now - lastMQTTAttempt >= MQTT_RETRY_MS) {
      lastMQTTAttempt = now;
      connectMQTT();
    }
  }

  if (mqtt.connected()) mqtt.loop();

  if (now - lastStatus >= STATUS_MS) {
    lastStatus = now;
    printStatus();
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'p' || c == 'P') printStatus();
    else if (c == 'r' || c == 'R') {
      WiFi.disconnect();
      delay(300);
      connectWiFi();
    }
    else if (c == 'm' || c == 'M') {
      mqtt.disconnect();
      connectMQTT();
    }
    else if (c == 'h' || c == 'H') printHelp();
  }

  delay(2);
}
