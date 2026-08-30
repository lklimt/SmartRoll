/*
  SmartRoll - TEST 5
  ESP32 + Hall A/B + ERTE RF + BH1750 + DS18B20

  Hall A OUT -> GPIO32
  Hall B OUT -> GPIO33
  RF DATA   -> GPIO25
  BH1750 SDA -> GPIO21
  BH1750 SCL -> GPIO22
  DS18B20 DATA -> GPIO27

  Serial Monitor: 115200 Bd

  Commands:
    u = UP
    s = STOP
    d = DOWN
    p = status
    r = reset Hall counters / relative position
    h = help

  IMPORTANT:
    RF implementation below is copied from the proven Test 3.
    Do not change its timing or payload during Test 5.
*/

#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RF433send.h>

const uint8_t HALL_A_PIN = 32;
const uint8_t HALL_B_PIN = 33;
const uint8_t RF_PIN = 25;
const uint8_t I2C_SDA_PIN = 21;
const uint8_t I2C_SCL_PIN = 22;
const uint8_t DS18B20_PIN = 27;

const int8_t transitionTable[16] = {
   0, -1, +1,  2,
  +1,  0,  2, -1,
  -1,  2,  0, +1,
   2, +1, -1,  0
};

volatile uint8_t previousState = 0;
volatile long position = 0;
volatile unsigned long cwCount = 0;
volatile unsigned long ccwCount = 0;
volatile unsigned long invalidCount = 0;
volatile unsigned long transitionCount = 0;

RfSend *tx_erte;
BH1750 lightMeter;
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);

const byte data_1nahoru[] = {0x33, 0x63, 0x13, 0x79, 0x08};
const byte data_1stop[]   = {0x33, 0x63, 0x13, 0x79, 0x2A};
const byte data_1dolu[]   = {0x33, 0x63, 0x13, 0x79, 0x19};

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

void sendCommand(const char *name, const char *command,
                 const byte *data, size_t length)
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
  Serial.print("RF send repetitions: ");
  Serial.println(repetitions);
}

void printStatus()
{
  long p;
  unsigned long cw, ccw, invalid, transitions;
  uint8_t state;

  noInterrupts();
  p = position;
  cw = cwCount;
  ccw = ccwCount;
  invalid = invalidCount;
  transitions = transitionCount;
  state = previousState;
  interrupts();

  float lux = lightMeter.readLightLevel();
  tempSensor.requestTemperatures();
  float temperature = tempSensor.getTempCByIndex(0);

  Serial.println("----------------------------------------");
  Serial.printf("Hall: A=%d B=%d  position=%ld  CW=%lu  CCW=%lu  transitions=%lu  invalid=%lu\n",
                (state >> 1) & 1, state & 1, p,
                cw, ccw, transitions, invalid);
  Serial.printf("BH1750: %.2f lx\n", lux);
  Serial.printf("DS18B20: %.2f C\n", temperature);
  Serial.println("RF: ready");
}

void printHelp()
{
  Serial.println();
  Serial.println("SmartRoll TEST 5 - ESP32 + Hall + RF + sensors");
  Serial.println("Commands: u=UP  s=STOP  d=DOWN  p=status  r=reset  h=help");
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  pinMode(HALL_A_PIN, INPUT);
  pinMode(HALL_B_PIN, INPUT);
  pinMode(RF_PIN, OUTPUT);
  digitalWrite(RF_PIN, LOW);

  previousState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  // Proven Test 3 RF configuration – unchanged.
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

  Serial.println();
  Serial.println("========================================");
  Serial.println("SmartRoll TEST 5");
  Serial.println("ESP32 + Hall + ERTE RF + BH1750 + DS18B20");
  Serial.println("========================================");
  printHelp();
}

void loop()
{
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "u" || command == "U") {
      sendCommand("UP", "01C", data_1nahoru, sizeof(data_1nahoru));
    }
    else if (command == "s" || command == "S") {
      sendCommand("STOP", "01B", data_1stop, sizeof(data_1stop));
    }
    else if (command == "d" || command == "D") {
      sendCommand("DOWN", "01A", data_1dolu, sizeof(data_1dolu));
    }
    else if (command == "p" || command == "P") {
      printStatus();
    }
    else if (command == "r" || command == "R") {
      noInterrupts();
      position = 0;
      cwCount = 0;
      ccwCount = 0;
      invalidCount = 0;
      transitionCount = 0;
      previousState = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
      interrupts();
      Serial.println("Hall counters / relative position reset.");
    }
    else if (command == "h" || command == "H") {
      printHelp();
    }
  }

  static unsigned long lastReport = 0;
  if (millis() - lastReport >= 2000) {
    lastReport = millis();
    printStatus();
  }
}
