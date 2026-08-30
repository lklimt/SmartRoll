/*
  SmartRoll - TEST 6
  Complete application firmware

  Validated subsystems:
    Hall A/B quadrature      GPIO32 / GPIO33
    ERTE RF DATA             GPIO25
    BH1750 I2C               GPIO21 / GPIO22
    DS18B20                  GPIO27
    PIR                      GPIO26 (PROVISIONAL)

  Network:
    Wi-Fi -> dedicated IoT network
    MQTT  -> Mosquitto -> Home Assistant

  IMPORTANT:
    Do not commit real Wi-Fi or MQTT credentials to GitHub.
    Replace the placeholders below locally before flashing.

  Serial Monitor: 115200 Bd
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RF433send.h>

// ============================================================
// USER CONFIGURATION - EDIT LOCALLY
// ============================================================
const char* WIFI_SSID     = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* MQTT_HOST     = "YOUR_MQTT_BROKER_IP";
const uint16_t MQTT_PORT  = 1883;
const char* MQTT_USER     = "YOUR_MQTT_USER";
const char* MQTT_PASSWORD = "YOUR_MQTT_PASSWORD";

const char* DEVICE_ID = "smartroll_01";
const char* DEVICE_NAME = "SmartRoll";
const char* FW_VERSION = "0.6.0-test6";

// ============================================================
// HARDWARE PINS - VALIDATED / PROVISIONAL AS DOCUMENTED
// ============================================================
const uint8_t HALL_A_PIN = 32;
const uint8_t HALL_B_PIN = 33;
const uint8_t RF_PIN = 25;
const uint8_t I2C_SDA_PIN = 21;
const uint8_t I2C_SCL_PIN = 22;
const uint8_t DS18B20_PIN = 27;
const uint8_t PIR_PIN = 26;  // PROVISIONAL

// ============================================================
// MQTT TOPICS
// ============================================================
String baseTopic;
String stateTopic;
String commandTopic;
String availabilityTopic;
String discoveryPrefix = "homeassistant";

// ============================================================
// OBJECTS
// ============================================================
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
BH1750 lightMeter;
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);
RfSend *tx_erte;

// ============================================================
// ERTE RF - EXACTLY THE PROVEN TEST 3 CONFIGURATION
// ============================================================
const byte data_1nahoru[] = {0x33, 0x63, 0x13, 0x79, 0x08};
const byte data_1stop[]   = {0x33, 0x63, 0x13, 0x79, 0x2A};
const byte data_1dolu[]   = {0x33, 0x63, 0x13, 0x79, 0x19};

// ============================================================
// HALL QUADRATURE DECODER
// Validated transition table from Tests 2-5.
// index = previous_state << 2 | current_state
// 0 = no movement, 1 = CW, -1 = CCW, 2 = invalid
// ============================================================
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

// ============================================================
// APPLICATION STATE
// ============================================================
enum MotionState {
  MOTION_STOPPED,
  MOTION_UP,
  MOTION_DOWN
};

MotionState motionState = MOTION_STOPPED;

float lastLux = NAN;
float lastTemperature = NAN;
bool pirState = false;

unsigned long lastSensorRead = 0;
unsigned long lastStatePublish = 0;
unsigned long lastWifiAttempt = 0;
unsigned long lastMqttAttempt = 0;

const unsigned long SENSOR_INTERVAL_MS = 5000;
const unsigned long STATE_INTERVAL_MS = 2000;
const unsigned long WIFI_RETRY_MS = 10000;
const unsigned long MQTT_RETRY_MS = 5000;

// ============================================================
// HALL ISR
// ============================================================
void IRAM_ATTR hallISR()
{
  uint8_t a = digitalRead(HALL_A_PIN);
  uint8_t b = digitalRead(HALL_B_PIN);
  uint8_t currentState = (a << 1) | b;

  if (currentState == previousState) return;

  uint8_t index = (previousState << 2) | currentState;
  int8_t movement = transitionTable[index];

  transitionCount++;

  if (movement == 1) {
    position++;
    cwCount++;
  }
  else if (movement == -1) {
    position--;
    ccwCount++;
  }
  else if (movement == 2) {
    invalidCount++;
  }

  previousState = currentState;
}

// ============================================================
// RF
// ============================================================
void sendRF(const char* name, const char* command,
            const byte* data, size_t length,
            MotionState newState)
{
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

// ============================================================
// MQTT HELPERS
// ============================================================
void publishRetained(const String& topic, const String& payload)
{
  if (mqtt.connected()) mqtt.publish(topic.c_str(), payload.c_str(), true);
}

void publishState()
{
  if (!mqtt.connected()) return;

  int32_t p;
  uint32_t cw, ccw, invalid, transitions;
  uint8_t state;

  noInterrupts();
  p = position;
  cw = cwCount;
  ccw = ccwCount;
  invalid = invalidCount;
  transitions = transitionCount;
  state = previousState;
  interrupts();

  String payload = "{";
  payload += "\"hall_a\":" + String((state >> 1) & 1) + ",";
  payload += "\"hall_b\":" + String(state & 1) + ",";
  payload += "\"position\":" + String(p) + ",";
  payload += "\"cw\":" + String(cw) + ",";
  payload += "\"ccw\":" + String(ccw) + ",";
  payload += "\"invalid\":" + String(invalid) + ",";
  payload += "\"transitions\":" + String(transitions) + ",";
  payload += "\"lux\":" + String(lastLux, 2) + ",";
  payload += "\"temperature\":" + String(lastTemperature, 2) + ",";
  payload += "\"pir\":" + String(pirState ? 1 : 0) + ",";
  payload += "\"rssi\":" + String(WiFi.RSSI());
  payload += "}";

  mqtt.publish(stateTopic.c_str(), payload.c_str(), true);

  String motion = "STOPPED";
  if (motionState == MOTION_UP) motion = "OPENING";
  if (motionState == MOTION_DOWN) motion = "CLOSING";
  mqtt.publish((baseTopic + "/cover/state").c_str(), motion.c_str(), true);

  mqtt.publish((baseTopic + "/direction/state").c_str(),
               directionText().c_str(), true);
}

String directionText()
{
  uint32_t cw, ccw;
  noInterrupts();
  cw = cwCount;
  ccw = ccwCount;
  interrupts();

  if (cw > ccw) return "CW";
  if (ccw > cw) return "CCW";
  return "UNKNOWN";
}

void publishDiscoverySensor(const String& component,
                            const String& objectId,
                            const String& name,
                            const String& stateTopicLocal,
                            const String& valueTemplate,
                            const String& unit,
                            const String& deviceClass)
{
  String topic = discoveryPrefix + "/" + component + "/" + DEVICE_ID + "/" + objectId + "/config";

  String payload = "{";
  payload += "\"name\":\"" + name + "\",";
  payload += "\"unique_id\":\"" + String(DEVICE_ID) + "_" + objectId + "\",";
  payload += "\"state_topic\":\"" + stateTopicLocal + "\",";
  payload += "\"value_template\":\"" + valueTemplate + "\",";
  if (unit.length()) payload += "\"unit_of_measurement\":\"" + unit + "\",";
  if (deviceClass.length()) payload += "\"device_class\":\"" + deviceClass + "\",";
  payload += "\"availability_topic\":\"" + availabilityTopic + "\",";
  payload += "\"payload_available\":\"online\",\"payload_not_available\":\"offline\",";
  payload += "\"device\":{";
  payload += "\"identifiers\":[\"" + String(DEVICE_ID) + "\"],";
  payload += "\"name\":\"" + String(DEVICE_NAME) + "\",";
  payload += "\"manufacturer\":\"SmartRoll\",";
  payload += "\"model\":\"ESP32 SmartRoll\",";
  payload += "\"sw_version\":\"" + String(FW_VERSION) + "\"";
  payload += "}}";

  publishRetained(topic, payload);
}

void publishDiscovery()
{
  if (!mqtt.connected()) return;

  // Cover
  String coverTopic = discoveryPrefix + "/cover/" + DEVICE_ID + "/cover/config";
  String cover = "{";
  cover += "\"name\":\"SmartRoll\",";
  cover += "\"unique_id\":\"" + String(DEVICE_ID) + "_cover\",";
  cover += "\"command_topic\":\"" + commandTopic + "\",";
  cover += "\"state_topic\":\"" + baseTopic + "/cover/state\",";
  cover += "\"payload_open\":\"UP\",\"payload_close\":\"DOWN\",\"payload_stop\":\"STOP\",";
  cover += "\"state_open\":\"OPEN\",\"state_closed\":\"CLOSED\",\"state_opening\":\"OPENING\",\"state_closing\":\"CLOSING\",\"state_stopped\":\"STOPPED\",";
  cover += "\"availability_topic\":\"" + availabilityTopic + "\",";
  cover += "\"payload_available\":\"online\",\"payload_not_available\":\"offline\",";
  cover += "\"device\":{";
  cover += "\"identifiers\":[\"" + String(DEVICE_ID) + "\"],";
  cover += "\"name\":\"" + String(DEVICE_NAME) + "\",";
  cover += "\"manufacturer\":\"SmartRoll\",";
  cover += "\"model\":\"ESP32 SmartRoll\",";
  cover += "\"sw_version\":\"" + String(FW_VERSION) + "\"";
  cover += "}}";
  publishRetained(coverTopic, cover);

  publishDiscoverySensor("sensor", "temperature", "SmartRoll Temperature",
                         stateTopic, "{{ value_json.temperature }}", "°C", "temperature");
  publishDiscoverySensor("sensor", "illuminance", "SmartRoll Illuminance",
                         stateTopic, "{{ value_json.lux }}", "lx", "illuminance");
  publishDiscoverySensor("sensor", "hall_position", "SmartRoll Hall Position",
                         stateTopic, "{{ value_json.position }}", "", "");
  publishDiscoverySensor("sensor", "hall_invalid", "SmartRoll Hall Invalid",
                         stateTopic, "{{ value_json.invalid }}", "", "");
  publishDiscoverySensor("sensor", "wifi_rssi", "SmartRoll Wi-Fi RSSI",
                         stateTopic, "{{ value_json.rssi }}", "dBm", "signal_strength");

  publishDiscoverySensor("sensor", "direction", "SmartRoll Direction",
                         baseTopic + "/direction/state", "{{ value }}", "", "");

  publishDiscoverySensor("binary_sensor", "pir", "SmartRoll Motion",
                         stateTopic, "{{ 'ON' if value_json.pir == 1 else 'OFF' }}", "", "motion");

  publishDiscoverySensor("sensor", "firmware", "SmartRoll Firmware",
                         baseTopic + "/firmware/state", "{{ value }}", "", "");
}

// ============================================================
// MQTT CALLBACK
// ============================================================
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

// ============================================================
// WIFI / MQTT
// ============================================================
void ensureWiFi()
{
  if (WiFi.status() == WL_CONNECTED) return;

  if (millis() - lastWifiAttempt < WIFI_RETRY_MS) return;
  lastWifiAttempt = millis();

  Serial.println("Wi-Fi: connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void ensureMQTT()
{
  if (WiFi.status() != WL_CONNECTED || mqtt.connected()) return;
  if (millis() - lastMqttAttempt < MQTT_RETRY_MS) return;
  lastMqttAttempt = millis();

  Serial.println("MQTT: connecting...");

  String clientId = String(DEVICE_ID) + "_" + String((uint32_t)ESP.getEfuseMac(), HEX);

  if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD,
                   availabilityTopic.c_str(), 0, true, "offline")) {
    Serial.println("MQTT: connected");
    mqtt.publish(availabilityTopic.c_str(), "online", true);
    mqtt.subscribe(commandTopic.c_str());
    publishDiscovery();
    mqtt.publish((baseTopic + "/firmware/state").c_str(), FW_VERSION, true);
    publishState();
  }
  else {
    Serial.print("MQTT: failed, state=");
    Serial.println(mqtt.state());
  }
}

// ============================================================
// SENSORS
// ============================================================
void readSensors()
{
  if (millis() - lastSensorRead < SENSOR_INTERVAL_MS) return;
  lastSensorRead = millis();

  lastLux = lightMeter.readLightLevel();

  tempSensor.requestTemperatures();
  lastTemperature = tempSensor.getTempCByIndex(0);

  pirState = digitalRead(PIR_PIN);
}

// ============================================================
// SERIAL
// ============================================================
void printStatus()
{
  int32_t p;
  uint32_t cw, ccw, invalid, transitions;
  uint8_t state;

  noInterrupts();
  p = position;
  cw = cwCount;
  ccw = ccwCount;
  invalid = invalidCount;
  transitions = transitionCount;
  state = previousState;
  interrupts();

  Serial.println("--------------------------------------------");
  Serial.printf("WiFi: %s  RSSI=%d dBm\n",
                WiFi.status() == WL_CONNECTED ? "connected" : "offline",
                WiFi.RSSI());
  Serial.printf("MQTT: %s\n", mqtt.connected() ? "connected" : "offline");
  Serial.printf("Hall: A=%d B=%d position=%ld CW=%lu CCW=%lu transitions=%lu invalid=%lu\n",
                (state >> 1) & 1, state & 1, (long)p,
                (unsigned long)cw, (unsigned long)ccw,
                (unsigned long)transitions, (unsigned long)invalid);
  Serial.printf("Direction: %s\n", directionText().c_str());
  Serial.printf("BH1750: %.2f lx\n", lastLux);
  Serial.printf("DS18B20: %.2f C\n", lastTemperature);
  Serial.printf("PIR: %s\n", pirState ? "MOTION" : "clear");
  Serial.printf("RF motion state: %d\n", (int)motionState);
}

void printHelp()
{
  Serial.println();
  Serial.println("SmartRoll TEST 6 - complete firmware");
  Serial.println("115200 Bd");
  Serial.println("Commands:");
  Serial.println("  u = UP");
  Serial.println("  s = STOP");
  Serial.println("  d = DOWN");
  Serial.println("  p = status");
  Serial.println("  r = reset relative Hall counters");
  Serial.println("  h = help");
  Serial.println();
}

// ============================================================
// SETUP
// ============================================================
void setup()
{
  Serial.begin(115200);
  delay(500);

  baseTopic = String("smartroll/") + DEVICE_ID;
  stateTopic = baseTopic + "/state";
  commandTopic = baseTopic + "/cover/command";
  availabilityTopic = baseTopic + "/availability";

  pinMode(HALL_A_PIN, INPUT);
  pinMode(HALL_B_PIN, INPUT);
  pinMode(RF_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(RF_PIN, LOW);

  previousState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  // Proven Test 3 RF configuration - unchanged.
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

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750: OK");
  } else {
    Serial.println("BH1750: ERROR / not detected");
  }

  tempSensor.begin();
  readSensors();

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(1024);

  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 6");
  Serial.println("Complete ESP32 firmware");
  Serial.println("Hall + ERTE RF + BH1750 + DS18B20 + PIR");
  Serial.println("Wi-Fi + MQTT + Home Assistant");
  Serial.println("============================================");
  printHelp();

  ensureWiFi();
}

// ============================================================
// LOOP
// ============================================================
void loop()
{
  ensureWiFi();
  ensureMQTT();

  if (mqtt.connected()) mqtt.loop();

  readSensors();

  if (millis() - lastStatePublish >= STATE_INTERVAL_MS) {
    lastStatePublish = millis();
    publishState();
    printStatus();
  }

  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {
      case 'u':
      case 'U':
        sendRF("UP", "01C", data_1nahoru, sizeof(data_1nahoru), MOTION_UP);
        break;

      case 's':
      case 'S':
        sendRF("STOP", "01B", data_1stop, sizeof(data_1stop), MOTION_STOPPED);
        break;

      case 'd':
      case 'D':
        sendRF("DOWN", "01A", data_1dolu, sizeof(data_1dolu), MOTION_DOWN);
        break;

      case 'p':
      case 'P':
        printStatus();
        break;

      case 'r':
      case 'R':
        noInterrupts();
        position = 0;
        cwCount = 0;
        ccwCount = 0;
        invalidCount = 0;
        transitionCount = 0;
        previousState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
        interrupts();
        Serial.println("Hall counters / relative position reset.");
        break;

      case 'h':
      case 'H':
        printHelp();
        break;
    }
  }
}
