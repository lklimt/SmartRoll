#include <Arduino.h>
#include <RF433send.h>

// SmartRoll Test 4
// ESP32 + Hall A/B quadrature + ERTE RF
// Hall A = GPIO32, Hall B = GPIO33
// RF DATA = GPIO25

const uint8_t HALL_A_PIN = 32;
const uint8_t HALL_B_PIN = 33;
const uint8_t RF_PIN = 25;

volatile int32_t position = 0;
volatile uint32_t cwCount = 0;
volatile uint32_t ccwCount = 0;
volatile uint32_t invalidCount = 0;
volatile uint8_t lastState = 0;

// Proven ControlRoll / ERTE timing configuration.
RF433send rf;

const uint8_t DATA_UP[]   = {0x33, 0x63, 0x13, 0x79, 0x08};
const uint8_t DATA_STOP[] = {0x33, 0x63, 0x13, 0x79, 0x2A};
const uint8_t DATA_DOWN[] = {0x33, 0x63, 0x13, 0x79, 0x19};

uint8_t readHallState() {
  return (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
}

void IRAM_ATTR hallISR() {
  uint8_t current = (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
  uint8_t transition = (lastState << 2) | current;

  switch (transition) {
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

    case 0b0000:
    case 0b0101:
    case 0b1010:
    case 0b1111:
      break;

    default:
      invalidCount++;
      break;
  }

  lastState = current;
}

void sendRF(const uint8_t *data, size_t len, const char *name) {
  // The exact RF setup/call below must remain identical to the proven
  // Test 3 firmware used for the selected RF433send library version.
  rf.send(data, len, RF_PIN);
  Serial.print("RF: ");
  Serial.print(name);
  Serial.println(" sent");
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

  Serial.printf("Hall: A=%d B=%d  state=%d\n", (state >> 1) & 1, state & 1, state);
  Serial.printf("Position: %ld  CW=%lu  CCW=%lu  Invalid=%lu\n",
                (long)pos, (unsigned long)cw, (unsigned long)ccw,
                (unsigned long)invalid);
}

void printHelp() {
  Serial.println();
  Serial.println("SmartRoll TEST 4 - ESP32 + Hall A/B + ERTE RF");
  Serial.println("115200 Bd");
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
  pinMode(RF_PIN, OUTPUT);

  lastState = readHallState();

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  Serial.println();
  Serial.println("============================================");
  Serial.println("SmartRoll TEST 4");
  Serial.println("ESP32 + Hall A/B + ERTE RF");
  Serial.println("============================================");
  Serial.printf("Initial Hall state: %d\n", lastState);
  printHelp();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {
      case 'u':
      case 'U':
        sendRF(DATA_UP, sizeof(DATA_UP), "UP");
        break;

      case 's':
      case 'S':
        sendRF(DATA_STOP, sizeof(DATA_STOP), "STOP");
        break;

      case 'd':
      case 'D':
        sendRF(DATA_DOWN, sizeof(DATA_DOWN), "DOWN");
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
