/*
  SmartRoll - TEST 4
  ESP32 + Hall A/B quadrature + ERTE FT45E RF

  Hall A = GPIO32
  Hall B = GPIO33
  RF DATA = GPIO25

  RF section intentionally uses the exact RF433send construction and
  send() call proven in Test 3. Do not change RF timing during Test 4.
*/

#include <Arduino.h>
#include <RF433send.h>

#define HALL_A_PIN 32
#define HALL_B_PIN 33
#define PIN_RFOUT 25

volatile int32_t position = 0;
volatile uint32_t cwCount = 0;
volatile uint32_t ccwCount = 0;
volatile uint32_t invalidCount = 0;
volatile uint8_t lastState = 0;

// IMPORTANT: same RF object/API as the proven Test 3 firmware.
RfSend *tx_erte;

const byte data_1nahoru[] = {0x33, 0x63, 0x13, 0x79, 0x08};
const byte data_1stop[]   = {0x33, 0x63, 0x13, 0x79, 0x2A};
const byte data_1dolu[]   = {0x33, 0x63, 0x13, 0x79, 0x19};

uint8_t readHallState() {
  return (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
}

void IRAM_ATTR hallISR() {
  uint8_t current = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
  uint8_t transition = (lastState << 2) | current;

  switch (transition) {
    // Same quadrature transition table as the approved ESP32 Hall test.
    case 0b0010:
    case 0b1011:
    case 0b1101:
    case 0b0100:
      position++;
      cwCount++;
      break;

    case 0b0001:
    case 0b0111:
    case 0b1110:
    case 0b1000:
      position--;
      ccwCount++;
      break;

    // No movement / unchanged state.
    case 0b0000:
    case 0b0101:
    case 0b1010:
    case 0b1111:
      break;

    // Illegal transition: both Hall bits changed at once.
    default:
      invalidCount++;
      break;
  }

  lastState = current;
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

  // EXACT same RF call as Test 3.
  byte repetitions = tx_erte->send(length, data);

  Serial.print("RF send repetitions: ");
  Serial.println(repetitions);
}

void printStatus() {
  uint32_t cw, ccw, invalid;
  int32_t pos;
  uint8_t state;

  noInterrupts();
  cw = cwCount;
  ccw = ccwCount;
  invalid = invalidCount;
  pos = position;
  state = lastState;
  interrupts();

  Serial.printf("Hall: A=%d B=%d  state=%d\n",
                (state >> 1) & 1, state & 1, state);
  Serial.printf("Position: %ld  CW=%lu  CCW=%lu  Invalid=%lu\n",
                (long)pos, (unsigned long)cw, (unsigned long)ccw,
                (unsigned long)invalid);
}

void printHelp() {
  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 4");
  Serial.println("ESP32 + Hall A/B + ERTE RF");
  Serial.println("============================================");
  Serial.println("Serial Monitor: 115200 Bd");
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  u = UP");
  Serial.println("  s = STOP");
  Serial.println("  d = DOWN");
  Serial.println("  r = reset relative counters/position");
  Serial.println("  p = print status");
  Serial.println("  h = help");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(HALL_A_PIN, INPUT);
  pinMode(HALL_B_PIN, INPUT);
  pinMode(PIN_RFOUT, OUTPUT);
  digitalWrite(PIN_RFOUT, LOW);

  lastState = readHallState();

  // EXACT RF setup copied from the proven Test 3 firmware.
  tx_erte = rfsend_builder(
      RfSendEncoding::TRIBIT,
      PIN_RFOUT,
      RFSEND_DEFAULT_CONVENTION,
      4,
      nullptr,
      9712,   // initseq
      5048,   // lo_prefix
      1472,   // hi_prefix
      0,      // first_lo_ign
      362,    // lo_short
      722,    // lo_long
      0,      // hi_short
      0,      // hi_long
      704,    // lo_last
      9360,   // separator
      39      // protocol parameter
  );

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  Serial.println();
  Serial.println("SmartRoll TEST 4 ready.");
  Serial.printf("Initial Hall state: %d\n", lastState);
  Serial.println("RF433send TRIBIT / Test 3 timing loaded.");
  printHelp();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {
      case 'u':
      case 'U':
        sendCommand("UP", "01C", data_1nahoru, sizeof(data_1nahoru));
        break;

      case 's':
      case 'S':
        sendCommand("STOP", "01B", data_1stop, sizeof(data_1stop));
        break;

      case 'd':
      case 'D':
        sendCommand("DOWN", "01A", data_1dolu, sizeof(data_1dolu));
        break;

      case 'r':
      case 'R':
        noInterrupts();
        position = 0;
        cwCount = 0;
        ccwCount = 0;
        invalidCount = 0;
        lastState = readHallState();
        interrupts();
        Serial.println("Counters and relative position reset.");
        break;

      case 'p':
      case 'P':
        printStatus();
        break;

      case 'h':
      case 'H':
        printHelp();
        break;
    }
  }

  static uint32_t lastReport = 0;
  if (millis() - lastReport >= 1000) {
    lastReport = millis();
    printStatus();
  }
}
