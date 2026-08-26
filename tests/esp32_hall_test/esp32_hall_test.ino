/*
  SmartRoll - TEST 2
  ESP32 + Hall A/B quadrature decoder

  Hardware:
    Hall A OUT -> GPIO32
    Hall B OUT -> GPIO33

  Supply:
    Hall VCC -> ESP32 3.3V
    Hall GND -> ESP32 GND

  Pull-ups:
    10k from GPIO32 to 3.3V
    10k from GPIO33 to 3.3V

  Purpose:
    Verify that the quadrature decoder previously validated
    on Arduino Nano works correctly on ESP32.

  Expected sequence:

    CW:
      11 -> 10 -> 00 -> 01 -> 11

    CCW:
      11 -> 01 -> 00 -> 10 -> 11

  IMPORTANT:
    Do not connect RF during this test.
*/

const uint8_t HALL_A_PIN = 32;
const uint8_t HALL_B_PIN = 33;

// State transition table.
// Index = previous_state * 4 + current_state
//
// States:
// 00 = 0
// 01 = 1
// 10 = 2
// 11 = 3
//
// +1 = CW
// -1 = CCW
//  0 = no movement
//  2 = invalid transition
const int8_t transitionTable[16] = {
   0,  -1,  +1,   2,
  +1,   0,   2,  -1,
  -1,   2,   0,  +1,
   2,  +1,  -1,   0
};

volatile uint8_t previousState = 0;
volatile long position = 0;
volatile unsigned long cwCount = 0;
volatile unsigned long ccwCount = 0;
volatile unsigned long invalidCount = 0;
volatile unsigned long transitionCount = 0;
volatile unsigned long lastTransitionUs = 0;
volatile unsigned long minTransitionUs = 0xFFFFFFFFUL;
volatile unsigned long maxTransitionUs = 0;

void IRAM_ATTR hallISR()
{
  uint8_t a = digitalRead(HALL_A_PIN);
  uint8_t b = digitalRead(HALL_B_PIN);
  uint8_t currentState = (a << 1) | b;

  if (currentState == previousState) {
    return;
  }

  uint8_t index = (previousState << 2) | currentState;
  int8_t movement = transitionTable[index];

  unsigned long now = micros();
  unsigned long dt = now - lastTransitionUs;
  lastTransitionUs = now;

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

  if (dt > 0) {
    if (dt < minTransitionUs) minTransitionUs = dt;
    if (dt > maxTransitionUs) maxTransitionUs = dt;
  }

  previousState = currentState;
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  pinMode(HALL_A_PIN, INPUT);
  pinMode(HALL_B_PIN, INPUT);

  uint8_t a = digitalRead(HALL_A_PIN);
  uint8_t b = digitalRead(HALL_B_PIN);
  previousState = (a << 1) | b;
  lastTransitionUs = micros();

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  Serial.println();
  Serial.println("=================================");
  Serial.println("SmartRoll TEST 2");
  Serial.println("ESP32 + Hall A/B quadrature");
  Serial.println("=================================");

  Serial.print("Initial state: ");
  Serial.println(previousState, BIN);

  Serial.println();
  Serial.println("Expected:");
  Serial.println("CW : 11 -> 10 -> 00 -> 01 -> 11");
  Serial.println("CCW: 11 -> 01 -> 00 -> 10 -> 11");
  Serial.println();
  Serial.println("Start rotation test...");
}

void loop()
{
  static unsigned long lastReport = 0;

  if (millis() - lastReport >= 1000) {
    lastReport = millis();

    noInterrupts();
    long p = position;
    unsigned long cw = cwCount;
    unsigned long ccw = ccwCount;
    unsigned long invalid = invalidCount;
    unsigned long transitions = transitionCount;
    unsigned long minDt = minTransitionUs;
    unsigned long maxDt = maxTransitionUs;
    uint8_t state = previousState;
    interrupts();

    Serial.print(millis());
    Serial.print(" ms  ");

    Serial.print("A=");
    Serial.print((state >> 1) & 1);
    Serial.print(" B=");
    Serial.print(state & 1);

    Serial.print("  position=");
    Serial.print(p);
    Serial.print("  CW=");
    Serial.print(cw);
    Serial.print("  CCW=");
    Serial.print(ccw);
    Serial.print("  transitions=");
    Serial.print(transitions);
    Serial.print("  invalid=");
    Serial.print(invalid);
    Serial.print("  min_dt_us=");

    if (minDt == 0xFFFFFFFFUL) Serial.print("-");
    else Serial.print(minDt);

    Serial.print("  max_dt_us=");
    Serial.println(maxDt);
  }
}
