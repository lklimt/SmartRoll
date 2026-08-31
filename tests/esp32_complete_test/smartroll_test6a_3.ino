/*
  SmartRoll - TEST 6A.3
  Complete integration test: ESP32 + Hall + ERTE RF + BH1750 + DS18B20 + Wi-Fi + MQTT

  Based on the validated Test 2-5 Hall/RF/sensor implementations and the
  successful Wi-Fi scan diagnostic from Test 6A.2.

  PIR is NOT part of SmartRoll.

  Serial Monitor: 115200 Bd
  Commands:
    u = UP
    d = DOWN
    s = STOP
    p = print complete status
    r = reset relative Hall counters
    h = help

  Do NOT store real Wi-Fi or MQTT credentials in GitHub.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RF433send.h>

// ---------- LOCAL CONFIGURATION ----------
const char* WIFI_SSID     = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_HOST     = "YOUR_MQTT_BROKER_IP";
const uint16_t MQTT_PORT  = 1883;
const char* MQTT_USER     = "YOUR_MQTT_USER";
const char* MQTT_PASSWORD = "YOUR_MQTT_PASSWORD";

const char* DEVICE_ID = "smartroll_01";
const char* FW_VERSION = "0.6.3-test6A";

// ---------- PINS ----------
const uint8_t HALL_A_PIN = 32;
const uint8_t HALL_B_PIN = 33;
const uint8_t RF_PIN = 25;
const uint8_t I2C_SDA_PIN = 21;
const uint8_t I2C_SCL_PIN = 22;
const uint8_t DS18B20_PIN = 27;

// ---------- RF ----------
const byte data_up[]   = {0x33, 0x63, 0x13, 0x79, 0x08};
const byte data_stop[] = {0x33, 0x63, 0x13, 0x79, 0x2A};
const byte data_down[] = {0x33, 0x63, 0x13, 0x79, 0x19};

RfSend* tx_erte = nullptr;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
BH1750 lightMeter;
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);

// ---------- HALL QUADRATURE ----------
const int8_t transitionTable[16] = {
   0, -1, +1,  2,
  +1,  0,  2, -1,
  -1,  2,  0, +1,
   2, +1, -1,  0
};

volatile uint8_t previousState = 0;
volatile int32_t position = 0;
volatile uint32_t cwCount = 0;
volatile uint32_t ccwCount = 0;
volatile uint32_t invalidCount = 0;
volatile uint32_t transitionCount = 0;
volatile int8_t lastDirection = 0;

// ---------- STATE ----------
enum MotionState { STOPPED, UP, DOWN };
MotionState motionState = STOPPED;

float lux = NAN;
float temperature = NAN;
bool bh1750Ok = false;
bool ds18b20Ok = false;

uint32_t lastSensorRead = 0;
uint32_t lastStatusPrint = 0;
uint32_t lastWifiAttempt = 0;
uint32_t lastMqttAttempt = 0;

const uint32_t SENSOR_INTERVAL_MS = 2000;
const uint32_t STATUS_INTERVAL_MS = 2000;
const uint32_t WIFI_RETRY_MS = 5000;
const uint32_t MQTT_RETRY_MS = 5000;

// ---------- HALL ISR ----------
void IRAM_ATTR hallISR()
{
  uint8_t currentState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
  if (currentState == previousState) return;

  uint8_t index = (previousState << 2) | currentState;
  int8_t movement = transitionTable[index];
  transitionCount++;

  if (movement == 1) {
    position++;
    cwCount++;
    lastDirection = 1;
  } else if (movement == -1) {
    position--;
    ccwCount++;
    lastDirection = -1;
  } else if (movement == 2) {
    invalidCount++;
  }

  previousState = currentState;
}

// ---------- RF ----------
void sendRF(const char* name, const char* code, const byte* data, size_t len, MotionState newState)
{
  Serial.printf("TX %s %s : ", code, name);
  for (size_t i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print('0');
    Serial.print(data[i], HEX);
    if (i + 1 < len) Serial.print(' ');
  }
  Serial.println();

  byte repetitions = tx_erte->send(len, data);
  motionState = newState;
  Serial.printf("RF send repetitions: %u\n", repetitions);
}

// ---------- SENSORS ----------
void readSensors()
{
  if (millis() - lastSensorRead < SENSOR_INTERVAL_MS) return;
  lastSensorRead = millis();

  if (bh1750Ok) {
    lux = lightMeter.readLightLevel();
  }

  if (ds18b20Ok) {
    tempSensor.requestTemperatures();
    temperature = tempSensor.getTempCByIndex(0);
  }
}

// ---------- STATUS ----------
void printStatus()
{
  int32_t p;
  uint32_t cw, ccw, inv, tr;
  uint8_t state;
  int8_t dir;

  noInterrupts();
  p = position;
  cw = cwCount;
  ccw = ccwCount;
  inv = invalidCount;
  tr = transitionCount;
  state = previousState;
  dir = lastDirection;
  interrupts();

  Serial.println("--------------------------------------------");
  Serial.printf("Wi-Fi: %s", WiFi.status() == WL_CONNECTED ? "CONNECTED" : "OFFLINE");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("  IP=%s RSSI=%d dBm", WiFi.localIP().toString().c_str(), WiFi.RSSI());
  }
  Serial.println();
  Serial.printf("MQTT: %s\n", mqtt.connected() ? "CONNECTED" : "OFFLINE");
  Serial.printf("Hall: A=%d B=%d position=%ld CW=%lu CCW=%lu transitions=%lu invalid=%lu\n",
                (state >> 1) & 1, state & 1, (long)p,
                (unsigned long)cw, (unsigned long)ccw,
                (unsigned long)tr, (unsigned long)inv);
  Serial.printf("Direction: %s\n", dir > 0 ? "CW" : dir < 0 ? "CCW" : "UNKNOWN");
  Serial.printf("BH1750: %.2f lx [%s]\n", lux, bh1750Ok ? "OK" : "ERROR");
  Serial.printf("DS18B20: %.2f C [%s]\n", temperature, ds18b20Ok ? "OK" : "ERROR");
  Serial.printf("Motion command state: %s\n",
                motionState == UP ? "UP" : motionState == DOWN ? "DOWN" : "STOPPED");
}

// ---------- MQTT ----------
String baseTopic;
String stateTopic;
String commandTopic;
String availabilityTopic;

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  msg.toUpperCase();

  if (msg == "UP") sendRF("UP", "01C", data_up, sizeof(data_up), UP);
  else if (msg == "DOWN") sendRF("DOWN", "01A", data_down, sizeof(data_down), DOWN);
  else if (msg == "STOP") sendRF("STOP", "01B", data_stop, sizeof(data_stop), STOPPED);
}

void connectWiFi()
{
  Serial.println("Wi-Fi: connection attempt");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void ensureWiFi()
{
  if (WiFi.status() == WL_CONNECTED) return;
  if (millis() - lastWifiAttempt < WIFI_RETRY_MS) return;
  lastWifiAttempt = millis();
  connectWiFi();
}

void ensureMQTT()
{
  if (WiFi.status() != WL_CONNECTED || mqtt.connected()) return;
  if (millis() - lastMqttAttempt < MQTT_RETRY_MS) return;
  lastMqttAttempt = millis();

  Serial.println("MQTT: connection attempt");
  String clientId = String(DEVICE_ID) + "_" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool ok;
  if (strlen(MQTT_USER) == 0) {
    ok = mqtt.connect(clientId.c_str(), availabilityTopic.c_str(), 0, true, "offline");
  } else {
    ok = mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD,
                      availabilityTopic.c_str(), 0, true, "offline");
  }

  if (ok) {
    Serial.println("MQTT: CONNECTED");
    mqtt.publish(availabilityTopic.c_str(), "online", true);
    mqtt.subscribe(commandTopic.c_str());
  } else {
    Serial.printf("MQTT: FAILED state=%d\n", mqtt.state());
  }
}

void publishState()
{
  if (!mqtt.connected()) return;

  int32_t p;
  uint32_t cw, ccw, inv, tr;
  uint8_t state;
  int8_t dir;

  noInterrupts();
  p = position;
  cw = cwCount;
  ccw = ccwCount;
  inv = invalidCount;
  tr = transitionCount;
  state = previousState;
  dir = lastDirection;
  interrupts();

  String payload = "{";
  payload += "\"hall_a\":" + String((state >> 1) & 1) + ",";
  payload += "\"hall_b\":" + String(state & 1) + ",";
  payload += "\"position\":" + String(p) + ",";
  payload += "\"cw\":" + String(cw) + ",";
  payload += "\"ccw\":" + String(ccw) + ",";
  payload += "\"transitions\":" + String(tr) + ",";
  payload += "\"invalid\":" + String(inv) + ",";
  payload += "\"direction\":\"" + String(dir > 0 ? "CW" : dir < 0 ? "CCW" : "UNKNOWN") + "\",";
  payload += "\"lux\":" + String(lux, 2) + ",";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"rssi\":" + String(WiFi.RSSI());
  payload += "}";

  mqtt.publish(stateTopic.c_str(), payload.c_str(), true);
}

// ---------- SERIAL ----------
void printHelp()
{
  Serial.println("Commands: u=UP d=DOWN s=STOP p=STATUS r=reset Hall h=HELP");
}

void handleSerial()
{
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'u' || c == 'U') sendRF("UP", "01C", data_up, sizeof(data_up), UP);
    else if (c == 'd' || c == 'D') sendRF("DOWN", "01A", data_down, sizeof(data_down), DOWN);
    else if (c == 's' || c == 'S') sendRF("STOP", "01B", data_stop, sizeof(data_stop), STOPPED);
    else if (c == 'p' || c == 'P') printStatus();
    else if (c == 'r' || c == 'R') {
      noInterrupts();
      position = 0;
      cwCount = 0;
      ccwCount = 0;
      invalidCount = 0;
      transitionCount = 0;
      lastDirection = 0;
      interrupts();
      Serial.println("Hall counters reset");
    }
    else if (c == 'h' || c == 'H') printHelp();
  }
}

// ---------- SETUP ----------
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 6A.3");
  Serial.println("ESP32 + Hall + ERTE RF + BH1750 + DS18B20 + Wi-Fi + MQTT");
  Serial.println("============================================");

  // Hall
  pinMode(HALL_A_PIN, INPUT_PULLUP);
  pinMode(HALL_B_PIN, INPUT_PULLUP);
  previousState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  // RF
  tx_erte = new RfSend(RF_PIN);
  Serial.println("RF: ready");

  // I2C / BH1750
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  bh1750Ok = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  Serial.printf("BH1750: %s\n", bh1750Ok ? "OK" : "ERROR");

  // DS18B20
  tempSensor.begin();
  ds18b20Ok = tempSensor.getDeviceCount() > 0;
  Serial.printf("DS18B20: %s (%u device(s))\n", ds18b20Ok ? "OK" : "ERROR", tempSensor.getDeviceCount());

  // Wi-Fi: same basic sequence proven by Test 6A.2
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

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Wi-Fi: CONNECTED  IP=%s RSSI=%d dBm\n",
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
  } else {
    Serial.printf("Wi-Fi: initial connection failed, status=%d\n", (int)WiFi.status());
  }

  baseTopic = String("smartroll/") + DEVICE_ID;
  stateTopic = baseTopic + "/state";
  commandTopic = baseTopic + "/command";
  availabilityTopic = baseTopic + "/availability";

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  printHelp();
  printStatus();
}

// ---------- LOOP ----------
void loop()
{
  handleSerial();
  ensureWiFi();
  ensureMQTT();

  if (mqtt.connected()) mqtt.loop();

  readSensors();

  if (millis() - lastStatusPrint >= STATUS_INTERVAL_MS) {
    lastStatusPrint = millis();
    printStatus();
    publishState();
  }

  delay(2);
}
