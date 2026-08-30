/*
  SmartRoll - TEST 6A
  Local complete integration firmware

  Purpose of 6A:
    - integrate all validated hardware functions on one ESP32
    - verify Hall quadrature + ERTE RF + BH1750 + DS18B20
    - verify Wi-Fi connection to the dedicated IoT network
    - verify basic MQTT connection/telemetry
    - keep control available from Arduino IDE Serial Monitor

  Home Assistant MQTT Discovery is deliberately NOT used in 6A.
  HA integration is the next stage (6B), after local operation is stable.

  PIR is NOT part of SmartRoll.

  Validated pin assignment:
    Hall A       GPIO32
    Hall B       GPIO33
    ERTE RF DATA GPIO25
    BH1750 SDA   GPIO21
    BH1750 SCL   GPIO22
    DS18B20 DATA GPIO27

  DS18B20 pull-up:
    4.7 kOhm preferred; 10 kOhm is acceptable for the laboratory test.

  Serial Monitor: 115200 Bd

  Serial commands:
    u = UP
    s = STOP
    d = DOWN
    p = status
    r = reset Hall counters / relative position
    h = help

  IMPORTANT:
    Never put real Wi-Fi or MQTT credentials into GitHub.
    Replace the placeholders locally before flashing.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RF433send.h>

// ================= CONFIGURATION =================
const char* WIFI_SSID     = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_HOST     = "YOUR_MQTT_BROKER_IP";
const uint16_t MQTT_PORT  = 1883;
const char* MQTT_USER     = "YOUR_MQTT_USER";
const char* MQTT_PASSWORD = "YOUR_MQTT_PASSWORD";

const char* DEVICE_ID   = "smartroll_01";
const char* DEVICE_NAME = "SmartRoll";
const char* FW_VERSION  = "0.6.1-test6a";

// ================= PINOUT =================
const uint8_t HALL_A_PIN = 32;
const uint8_t HALL_B_PIN = 33;
const uint8_t RF_PIN = 25;
const uint8_t I2C_SDA_PIN = 21;
const uint8_t I2C_SCL_PIN = 22;
const uint8_t DS18B20_PIN = 27;

// ================= MQTT =================
String baseTopic;
String stateTopic;
String commandTopic;
String availabilityTopic;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
BH1750 lightMeter;
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);
RfSend *tx_erte = nullptr;

// ================= ERTE RF =================
// This is the tested Test 3 RF protocol and timing. Do not alter here.
const byte data_1nahoru[] = {0x33, 0x63, 0x13, 0x79, 0x08};
const byte data_1stop[]   = {0x33, 0x63, 0x13, 0x79, 0x2A};
const byte data_1dolu[]   = {0x33, 0x63, 0x13, 0x79, 0x19};

// ================= HALL QUADRATURE =================
// Index = previous_state * 4 + current_state
// +1 = CW, -1 = CCW, 2 = invalid transition, 0 = no movement
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
volatile int8_t lastDirection = 0;  // +1 CW, -1 CCW, 0 unknown

// ================= APPLICATION STATE =================
enum MotionState {
  MOTION_STOPPED,
  MOTION_UP,
  MOTION_DOWN
};

MotionState motionState = MOTION_STOPPED;

float lastLux = NAN;
float lastTemperature = NAN;
bool bh1750Ok = false;
bool ds18b20Ok = false;

uint32_t lastSensorRead = 0;
uint32_t lastPublish = 0;
uint32_t lastStatus = 0;
uint32_t lastWifiAttempt = 0;
uint32_t lastMqttAttempt = 0;

const uint32_t SENSOR_INTERVAL_MS = 5000;
const uint32_t PUBLISH_INTERVAL_MS = 2000;
const uint32_t STATUS_INTERVAL_MS = 2000;
const uint32_t WIFI_RETRY_MS = 10000;
const uint32_t MQTT_RETRY_MS = 5000;

// ================= HALL ISR =================
void IRAM_ATTR hallISR()
{
  uint8_t currentState = (digitalRead(HALL_A_PIN) << 1) |
                         digitalRead(HALL_B_PIN);

  if (currentState == previousState) return;

  uint8_t index = (previousState << 2) | currentState;
  int8_t movement = transitionTable[index];
  transitionCount++;

  if (movement == 1) {
    position++;
    cwCount++;
    lastDirection = 1;
  }
  else if (movement == -1) {
    position--;
    ccwCount++;
    lastDirection = -1;
  }
  else if (movement == 2) {
    invalidCount++;
  }

  previousState = currentState;
}

// ================= RF =================
void sendRF(const char* name, const char* command,
            const byte* data, size_t length, MotionState newState)
{
  if (tx_erte == nullptr) {
    Serial.println("RF ERROR: transmitter not initialized");
    return;
  }

  Serial.print("TX ");
  Serial.print(command);
  Serial.print(" ");
  Serial.print(name);
  Serial.print(" : ");

  for (size_t i = 0; i < length; i++) {
    if (data[i] < 0x10) Serial.print('0');
    Serial.print(data[i], HEX);
    if (i + 1 < length) Serial.print(' ');
  }
  Serial.println();

  byte repetitions = tx_erte->send(length, data);
  motionState = newState;

  Serial.print("RF send repetitions: ");
  Serial.println(repetitions);
}

// ================= SENSORS =================
void readSensors()
{
  if (millis() - lastSensorRead < SENSOR_INTERVAL_MS) return;
  lastSensorRead = millis();

  if (bh1750Ok) {
    float lux = lightMeter.readLightLevel();
    if (lux >= 0) lastLux = lux;
  }

  if (ds18b20Ok) {
    tempSensor.requestTemperatures();
    float t = tempSensor.getTempCByIndex(0);
    if (t != DEVICE_DISCONNECTED_C && t > -100 && t < 125) {
      lastTemperature = t;
    }
  }
}

// ================= STATUS SNAPSHOT =================
void getHallSnapshot(int32_t &p, uint32_t &cw, uint32_t &ccw,
                     uint32_t &invalid, uint32_t &transitions,
                     uint8_t &state, int8_t &dir)
{
  noInterrupts();
  p = position;
  cw = cwCount;
  ccw = ccwCount;
  invalid = invalidCount;
  transitions = transitionCount;
  state = previousState;
  dir = lastDirection;
  interrupts();
}

const char* directionName(int8_t dir)
{
  if (dir > 0) return "CW";
  if (dir < 0) return "CCW";
  return "UNKNOWN";
}

const char* motionName()
{
  if (motionState == MOTION_UP) return "UP";
  if (motionState == MOTION_DOWN) return "DOWN";
  return "STOPPED";
}

// ================= MQTT =================
void publishState()
{
  if (!mqtt.connected()) return;

  int32_t p;
  uint32_t cw, ccw, invalid, transitions;
  uint8_t state;
  int8_t dir;
  getHallSnapshot(p, cw, ccw, invalid, transitions, state, dir);

  String payload = "{";
  payload += "\"hall_a\":" + String((state >> 1) & 1) + ",";
  payload += "\"hall_b\":" + String(state & 1) + ",";
  payload += "\"position\":" + String(p) + ",";
  payload += "\"cw\":" + String(cw) + ",";
  payload += "\"ccw\":" + String(ccw) + ",";
  payload += "\"invalid\":" + String(invalid) + ",";
  payload += "\"transitions\":" + String(transitions) + ",";
  payload += "\"direction\":\"" + String(directionName(dir)) + "\",";
  payload += "\"motion\":\"" + String(motionName()) + "\",";
  payload += "\"lux\":" + String(lastLux, 2) + ",";
  payload += "\"temperature\":" + String(lastTemperature, 2) + ",";
  payload += "\"rssi\":" + String(WiFi.RSSI());
  payload += "}";

  mqtt.publish(stateTopic.c_str(), payload.c_str(), true);
}

void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  String message;
  for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
  message.trim();
  message.toUpperCase();

  if (message == "UP") {
    sendRF("UP", "01C", data_1nahoru, sizeof(data_1nahoru), MOTION_UP);
  }
  else if (message == "DOWN") {
    sendRF("DOWN", "01A", data_1dolu, sizeof(data_1dolu), MOTION_DOWN);
  }
  else if (message == "STOP") {
    sendRF("STOP", "01B", data_1stop, sizeof(data_1stop), MOTION_STOPPED);
  }
}

// ================= WI-FI =================
void ensureWiFi()
{
  if (WiFi.status() == WL_CONNECTED) return;
  if (millis() - lastWifiAttempt < WIFI_RETRY_MS) return;
  lastWifiAttempt = millis();

  Serial.println("Wi-Fi: connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// ================= MQTT CONNECTION =================
void ensureMQTT()
{
  if (WiFi.status() != WL_CONNECTED || mqtt.connected()) return;
  if (millis() - lastMqttAttempt < MQTT_RETRY_MS) return;
  lastMqttAttempt = millis();

  Serial.println("MQTT: connecting...");

  String clientId = String(DEVICE_ID) + "_" +
                    String((uint32_t)ESP.getEfuseMac(), HEX);

  if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD,
                   availabilityTopic.c_str(), 0, true, "offline")) {
    Serial.println("MQTT: connected");
    mqtt.publish(availabilityTopic.c_str(), "online", true);
    mqtt.subscribe(commandTopic.c_str());
    mqtt.publish((baseTopic + "/firmware/state").c_str(), FW_VERSION, true);
    publishState();
  }
  else {
    Serial.print("MQTT: failed, state=");
    Serial.println(mqtt.state());
  }
}

// ================= SERIAL STATUS =================
void printStatus()
{
  int32_t p;
  uint32_t cw, ccw, invalid, transitions;
  uint8_t state;
  int8_t dir;
  getHallSnapshot(p, cw, ccw, invalid, transitions, state, dir);

  Serial.println("--------------------------------------------");
  Serial.printf("WiFi: %s  RSSI=%d dBm\n",
                WiFi.status() == WL_CONNECTED ? "connected" : "offline",
                WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0);
  Serial.printf("MQTT: %s\n", mqtt.connected() ? "connected" : "offline");
  Serial.printf("Hall: A=%d B=%d position=%ld CW=%lu CCW=%lu transitions=%lu invalid=%lu\n",
                (state >> 1) & 1, state & 1,
                (long)p,
                (unsigned long)cw,
                (unsigned long)ccw,
                (unsigned long)transitions,
                (unsigned long)invalid);
  Serial.printf("Direction: %s  RF motion: %s\n",
                directionName(dir), motionName());
  Serial.printf("BH1750: %.2f lx (%s)\n",
                lastLux, bh1750Ok ? "OK" : "ERROR");
  Serial.printf("DS18B20: %.2f C (%s)\n",
                lastTemperature, ds18b20Ok ? "OK" : "ERROR");
}

void printHelp()
{
  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 6A");
  Serial.println("ESP32 + Hall + ERTE RF + BH1750 + DS18B20");
  Serial.println("Wi-Fi + basic MQTT telemetry");
  Serial.println("PIR: NOT USED");
  Serial.println("============================================");
  Serial.println("Serial Monitor: 115200 Bd");
  Serial.println("Commands:");
  Serial.println("  u = UP");
  Serial.println("  s = STOP");
  Serial.println("  d = DOWN");
  Serial.println("  p = status");
  Serial.println("  r = reset Hall counters / relative position");
  Serial.println("  h = help");
  Serial.println();
  Serial.println("MQTT command topic:");
  Serial.println("  smartroll/smartroll_01/cover/command");
  Serial.println("MQTT state topic:");
  Serial.println("  smartroll/smartroll_01/state");
  Serial.println();
}

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);
  delay(500);

  baseTopic = String("smartroll/") + DEVICE_ID;
  stateTopic = baseTopic + "/state";
  commandTopic = baseTopic + "/cover/command";
  availabilityTopic = baseTopic + "/availability";

  // Hall inputs
  pinMode(HALL_A_PIN, INPUT);
  pinMode(HALL_B_PIN, INPUT);

  // RF output
  pinMode(RF_PIN, OUTPUT);
  digitalWrite(RF_PIN, LOW);

  // Establish the initial Hall state BEFORE enabling interrupts.
  previousState = (digitalRead(HALL_A_PIN) << 1) |
                  digitalRead(HALL_B_PIN);

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  // EXACT RF configuration from the proven Test 3 firmware.
  tx_erte = rfsend_builder(
      RfSendEncoding::TRIBIT,
      RF_PIN,
      RFSEND_DEFAULT_CONVENTION,
      4,
      nullptr,
      9712,
      5048,
      1472,
      0,
      362,
      722,
      0,
      0,
      704,
      9360,
      39
  );

  // I2C sensors
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  bh1750Ok = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  // DS18B20
  tempSensor.begin();
  ds18b20Ok = (tempSensor.getDeviceCount() > 0);

  // MQTT
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);

  Serial.println();
  Serial.println("SmartRoll TEST 6A boot");
  Serial.printf("Firmware: %s\n", FW_VERSION);
  Serial.printf("BH1750: %s\n", bh1750Ok ? "OK" : "ERROR / not detected");
  Serial.printf("DS18B20: %s\n", ds18b20Ok ? "OK" : "ERROR / not detected");
  Serial.printf("Initial Hall state: %d\n", previousState);
  printHelp();

  ensureWiFi();
}

// ================= LOOP =================
void loop()
{
  // These functions are intentionally non-blocking.
  ensureWiFi();
  ensureMQTT();

  if (mqtt.connected()) {
    mqtt.loop();
  }

  readSensors();

  if (millis() - lastPublish >= PUBLISH_INTERVAL_MS) {
    lastPublish = millis();
    publishState();
  }

  if (millis() - lastStatus >= STATUS_INTERVAL_MS) {
    lastStatus = millis();
    printStatus();
  }

  // Local laboratory control from Arduino IDE Serial Monitor.
  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {
      case 'u': case 'U':
        sendRF("UP", "01C", data_1nahoru, sizeof(data_1nahoru), MOTION_UP);
        break;

      case 's': case 'S':
        sendRF("STOP", "01B", data_1stop, sizeof(data_1stop), MOTION_STOPPED);
        break;

      case 'd': case 'D':
        sendRF("DOWN", "01A", data_1dolu, sizeof(data_1dolu), MOTION_DOWN);
        break;

      case 'p': case 'P':
        printStatus();
        break;

      case 'r': case 'R':
        noInterrupts();
        position = 0;
        cwCount = 0;
        ccwCount = 0;
        invalidCount = 0;
        transitionCount = 0;
        lastDirection = 0;
        previousState = (digitalRead(HALL_A_PIN) << 1) |
                        digitalRead(HALL_B_PIN);
        interrupts();
        Serial.println("Hall counters / relative position reset.");
        break;

      case 'h': case 'H':
        printHelp();
        break;
    }
  }
}
