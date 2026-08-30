/*
  SmartRoll - TEST 6
  Complete application firmware - first integration build

  Validated subsystems:
    Hall A/B quadrature      GPIO32 / GPIO33
    ERTE RF DATA             GPIO25
    BH1750 I2C               GPIO21 / GPIO22
    DS18B20                  GPIO27
    Wi-Fi                    dedicated IoT network
    MQTT                     Mosquitto -> Home Assistant

  PIR is NOT part of SmartRoll and is deliberately excluded.

  Serial Monitor: 115200 Bd

  Serial commands:
    u = UP
    s = STOP
    d = DOWN
    p = print status
    r = reset relative Hall counters
    h = help

  IMPORTANT:
    Do not commit real Wi-Fi or MQTT credentials to GitHub.
    Replace the placeholders below locally before flashing.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RF433send.h>

// ---------- local configuration ----------
const char* WIFI_SSID     = "YOUR_IOT_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_HOST     = "YOUR_MQTT_BROKER_IP";
const uint16_t MQTT_PORT  = 1883;
const char* MQTT_USER     = "YOUR_MQTT_USER";
const char* MQTT_PASSWORD = "YOUR_MQTT_PASSWORD";

const char* DEVICE_ID   = "smartroll_01";
const char* DEVICE_NAME = "SmartRoll";
const char* FW_VERSION  = "0.6.1-test6";

// ---------- validated pins ----------
const uint8_t HALL_A_PIN = 32;
const uint8_t HALL_B_PIN = 33;
const uint8_t RF_PIN = 25;
const uint8_t I2C_SDA_PIN = 21;
const uint8_t I2C_SCL_PIN = 22;
const uint8_t DS18B20_PIN = 27;

// ---------- MQTT ----------
String baseTopic;
String stateTopic;
String commandTopic;
String availabilityTopic;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
BH1750 lightMeter;
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);
RfSend *tx_erte;

// ---------- ERTE RF: copied unchanged from proven Test 3 ----------
const byte data_1nahoru[] = {0x33, 0x63, 0x13, 0x79, 0x08};
const byte data_1stop[]   = {0x33, 0x63, 0x13, 0x79, 0x2A};
const byte data_1dolu[]   = {0x33, 0x63, 0x13, 0x79, 0x19};

// ---------- Hall quadrature: validated Tests 2-5 ----------
// index = previous_state * 4 + current_state
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
volatile int8_t lastDirection = 0; // +1 CW, -1 CCW, 0 unknown

// ---------- application state ----------
enum MotionState { MOTION_STOPPED, MOTION_UP, MOTION_DOWN };
MotionState motionState = MOTION_STOPPED;

float lastLux = NAN;
float lastTemperature = NAN;
bool bh1750Ok = false;
bool ds18b20Ok = false;

uint32_t lastSensorRead = 0;
uint32_t lastPublish = 0;
uint32_t lastWifiAttempt = 0;
uint32_t lastMqttAttempt = 0;

const uint32_t SENSOR_INTERVAL_MS = 5000;
const uint32_t PUBLISH_INTERVAL_MS = 2000;
const uint32_t WIFI_RETRY_MS = 10000;
const uint32_t MQTT_RETRY_MS = 5000;

String directionText()
{
  int8_t d;
  noInterrupts();
  d = lastDirection;
  interrupts();
  if (d > 0) return "CW";
  if (d < 0) return "CCW";
  return "UNKNOWN";
}

// ---------- Hall ISR: only fast local work ----------
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
  } else if (movement == -1) {
    position--;
    ccwCount++;
    lastDirection = -1;
  } else if (movement == 2) {
    invalidCount++;
  }

  previousState = currentState;
}

// ---------- ERTE RF ----------
void sendRF(const char* name, const char* command,
            const byte* data, size_t length, MotionState newState)
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

// ---------- sensors ----------
void readSensors()
{
  if (millis() - lastSensorRead < SENSOR_INTERVAL_MS) return;
  lastSensorRead = millis();

  if (bh1750Ok) lastLux = lightMeter.readLightLevel();

  if (ds18b20Ok) {
    tempSensor.requestTemperatures();
    lastTemperature = tempSensor.getTempCByIndex(0);
  }
}

// ---------- MQTT state ----------
void publishState()
{
  if (!mqtt.connected()) return;

  int32_t p;
  uint32_t cw, ccw, invalid, transitions;
  uint8_t state;
  int8_t dir;

  noInterrupts();
  p = position;
  cw = cwCount;
  ccw = ccwCount;
  invalid = invalidCount;
  transitions = transitionCount;
  state = previousState;
  dir = lastDirection;
  interrupts();

  String payload = "{";
  payload += "\"hall_a\":" + String((state >> 1) & 1) + ",";
  payload += "\"hall_b\":" + String(state & 1) + ",";
  payload += "\"position\":" + String(p) + ",";
  payload += "\"cw\":" + String(cw) + ",";
  payload += "\"ccw\":" + String(ccw) + ",";
  payload += "\"invalid\":" + String(invalid) + ",";
  payload += "\"transitions\":" + String(transitions) + ",";
  payload += "\"direction\":\"" + (dir > 0 ? String("CW") : dir < 0 ? String("CCW") : String("UNKNOWN")) + "\",";
  payload += "\"lux\":" + String(lastLux, 2) + ",";
  payload += "\"temperature\":" + String(lastTemperature, 2) + ",";
  payload += "\"rssi\":" + String(WiFi.RSSI());
  payload += "}";

  mqtt.publish(stateTopic.c_str(), payload.c_str(), true);

  String coverState = "STOPPED";
  if (motionState == MOTION_UP) coverState = "OPENING";
  if (motionState == MOTION_DOWN) coverState = "CLOSING";
  mqtt.publish((baseTopic + "/cover/state").c_str(), coverState.c_str(), true);
}

void discoverySensor(const char* component, const char* objectId,
                     const char* name, const char* valueTemplate,
                     const char* unit, const char* deviceClass)
{
  String topic = String("homeassistant/") + component + "/" + DEVICE_ID + "/" + objectId + "/config";
  String payload = "{";
  payload += "\"name\":\"" + String(name) + "\",";
  payload += "\"unique_id\":\"" + String(DEVICE_ID) + "_" + objectId + "\",";
  payload += "\"state_topic\":\"" + stateTopic + "\",";
  payload += "\"value_template\":\"" + String(valueTemplate) + "\",";
  if (strlen(unit)) payload += "\"unit_of_measurement\":\"" + String(unit) + "\",";
  if (strlen(deviceClass)) payload += "\"device_class\":\"" + String(deviceClass) + "\",";
  payload += "\"availability_topic\":\"" + availabilityTopic + "\",";
  payload += "\"payload_available\":\"online\",\"payload_not_available\":\"offline\",";
  payload += "\"device\":{";
  payload += "\"identifiers\":[\"" + String(DEVICE_ID) + "\"],";
  payload += "\"name\":\"" + String(DEVICE_NAME) + "\",";
  payload += "\"manufacturer\":\"SmartRoll\",";
  payload += "\"model\":\"ESP32 SmartRoll\",";
  payload += "\"sw_version\":\"" + String(FW_VERSION) + "\"";
  payload += "}}";
  mqtt.publish(topic.c_str(), payload.c_str(), true);
}

void publishDiscovery()
{
  String topic = String("homeassistant/cover/") + DEVICE_ID + "/cover/config";
  String payload = "{";
  payload += "\"name\":\"SmartRoll\",";
  payload += "\"unique_id\":\"" + String(DEVICE_ID) + "_cover\",";
  payload += "\"command_topic\":\"" + commandTopic + "\",";
  payload += "\"state_topic\":\"" + baseTopic + "/cover/state\",";
  payload += "\"payload_open\":\"UP\",\"payload_close\":\"DOWN\",\"payload_stop\":\"STOP\",";
  payload += "\"state_opening\":\"OPENING\",\"state_closing\":\"CLOSING\",\"state_stopped\":\"STOPPED\",";
  payload += "\"availability_topic\":\"" + availabilityTopic + "\",";
  payload += "\"payload_available\":\"online\",\"payload_not_available\":\"offline\",";
  payload += "\"device\":{";
  payload += "\"identifiers\":[\"" + String(DEVICE_ID) + "\"],";
  payload += "\"name\":\"" + String(DEVICE_NAME) + "\",";
  payload += "\"manufacturer\":\"SmartRoll\",";
  payload += "\"model\":\"ESP32 SmartRoll\",";
  payload += "\"sw_version\":\"" + String(FW_VERSION) + "\"";
  payload += "}}";
  mqtt.publish(topic.c_str(), payload.c_str(), true);

  discoverySensor("sensor", "temperature", "SmartRoll Temperature", "{{ value_json.temperature }}", "°C", "temperature");
  discoverySensor("sensor", "illuminance", "SmartRoll Illuminance", "{{ value_json.lux }}", "lx", "illuminance");
  discoverySensor("sensor", "hall_position", "SmartRoll Hall Position", "{{ value_json.position }}", "", "");
  discoverySensor("sensor", "hall_invalid", "SmartRoll Hall Invalid", "{{ value_json.invalid }}", "", "");
  discoverySensor("sensor", "direction", "SmartRoll Direction", "{{ value_json.direction }}", "", "");
  discoverySensor("sensor", "wifi_rssi", "SmartRoll Wi-Fi RSSI", "{{ value_json.rssi }}", "dBm", "signal_strength");
  discoverySensor("sensor", "firmware", "SmartRoll Firmware", "{{ value }}", "", "");
}

// ---------- MQTT ----------
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  String message;
  for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
  message.trim();
  message.toUpperCase();

  if (message == "UP") sendRF("UP", "01C", data_1nahoru, sizeof(data_1nahoru), MOTION_UP);
  else if (message == "DOWN") sendRF("DOWN", "01A", data_1dolu, sizeof(data_1dolu), MOTION_DOWN);
  else if (message == "STOP") sendRF("STOP", "01B", data_1stop, sizeof(data_1stop), MOTION_STOPPED);
}

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
  } else {
    Serial.print("MQTT: failed, state=");
    Serial.println(mqtt.state());
  }
}

// ---------- Serial ----------
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
  Serial.printf("WiFi: %s  RSSI=%d dBm\n", WiFi.status() == WL_CONNECTED ? "connected" : "offline", WiFi.RSSI());
  Serial.printf("MQTT: %s\n", mqtt.connected() ? "connected" : "offline");
  Serial.printf("Hall: A=%d B=%d position=%ld CW=%lu CCW=%lu transitions=%lu invalid=%lu\n",
                (state >> 1) & 1, state & 1, (long)p,
                (unsigned long)cw, (unsigned long)ccw,
                (unsigned long)transitions, (unsigned long)invalid);
  Serial.printf("Direction: %s\n", directionText().c_str());
  Serial.printf("BH1750: %.2f lx (%s)\n", lastLux, bh1750Ok ? "OK" : "ERROR");
  Serial.printf("DS18B20: %.2f C (%s)\n", lastTemperature, ds18b20Ok ? "OK" : "ERROR");
  Serial.printf("RF motion state: %d\n", (int)motionState);
}

void printHelp()
{
  Serial.println();
  Serial.println("SmartRoll TEST 6 - complete firmware");
  Serial.println("Serial Monitor: 115200 Bd");
  Serial.println("Commands: u=UP  s=STOP  d=DOWN  p=status  r=reset  h=help");
  Serial.println("MQTT: smartroll/smartroll_01/cover/command");
  Serial.println();
}

// ---------- setup ----------
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
  digitalWrite(RF_PIN, LOW);

  previousState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  // EXACT Test 3 RF configuration - do not change.
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
  bh1750Ok = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  tempSensor.begin();
  ds18b20Ok = (tempSensor.getDeviceCount() > 0);

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(1024);

  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 6");
  Serial.println("ESP32 + Hall + ERTE RF + BH1750 + DS18B20");
  Serial.println("Wi-Fi + MQTT + Home Assistant");
  Serial.println("PIR: NOT USED");
  Serial.println("============================================");
  Serial.printf("BH1750: %s\n", bh1750Ok ? "OK" : "ERROR / not detected");
  Serial.printf("DS18B20: %s\n", ds18b20Ok ? "OK" : "ERROR / not detected");
  printHelp();

  ensureWiFi();
}

// ---------- loop ----------
void loop()
{
  ensureWiFi();
  ensureMQTT();
  if (mqtt.connected()) mqtt.loop();

  readSensors();

  if (millis() - lastPublish >= PUBLISH_INTERVAL_MS) {
    lastPublish = millis();
    publishState();
    printStatus();
  }

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
        previousState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
        interrupts();
        Serial.println("Hall counters / relative position reset.");
        break;
      case 'h': case 'H':
        printHelp();
        break;
    }
  }
}
