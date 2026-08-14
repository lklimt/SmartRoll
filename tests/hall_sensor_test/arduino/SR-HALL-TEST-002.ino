/*
  SmartRoll – SR-HALL-TEST-002
  Arduino Nano / ATmega328P
  2x A3144 Hall sensors

  Wiring:
    Hall A OUT -> D2
    Hall B OUT -> D3
    Hall A/B VCC -> 5V
    Hall A/B GND -> GND
    10k pull-up: each OUT -> 5V
    100nF: each VCC -> GND

  Purpose:
    - verify both Hall sensors
    - record A/B state transitions
    - determine transition sequence in both rotation directions
    - count transitions and invalid states

  Serial output: 115200 baud

  Expected valid quadrature state transitions:
    00 -> 01 -> 11 -> 10 -> 00
  or
    00 -> 10 -> 11 -> 01 -> 00

  IMPORTANT:
    The A/B sequence is measured experimentally. Do not yet assign
    clockwise/counter-clockwise to WIND/UNWIND in firmware.
*/

const byte HALL_A_PIN = 2;
const byte HALL_B_PIN = 3;

volatile byte hallA = 0;
volatile byte hallB = 0;
volatile byte previousState = 0;
volatile unsigned long transitionCount = 0;
volatile unsigned long invalidCount = 0;
volatile unsigned long lastTransitionMicros = 0;
volatile unsigned long pulsePeriodMicros = 0;

// Lookup table for quadrature transitions.
// Index = previous_state << 2 | new_state
// States: 0=00, 1=01, 2=10, 3=11
// Valid transitions are +/-1 in the Gray-code ring.
const int8_t transitionTable[16] = {
   0,  +1,  -1,   0,
  -1,   0,   0,  +1,
  +1,   0,   0,  -1,
   0,  -1,  +1,   0
};

void printState(byte state) {
  Serial.print((state >> 1) & 1); // A
  Serial.print((state >> 0) & 1); // B
}

byte readHallState() {
  // A3144 output is normally LOW when active and HIGH when inactive.
  // We deliberately report the electrical logic level directly.
  byte a = digitalRead(HALL_A_PIN) ? 1 : 0;
  byte b = digitalRead(HALL_B_PIN) ? 1 : 0;
  return (a << 1) | b;
}

void hallISR() {
  byte newState = readHallState();
  byte index = (previousState << 2) | newState;
  int8_t step = transitionTable[index];

  unsigned long now = micros();
  pulsePeriodMicros = now - lastTransitionMicros;
  lastTransitionMicros = now;

  if (step != 0) {
    transitionCount++;
  } else if (newState != previousState) {
    invalidCount++;
  }

  previousState = newState;
  hallA = (newState >> 1) & 1;
  hallB = newState & 1;
}

void setup() {
  Serial.begin(115200);

  // External 10k pull-ups are specified in the wiring document.
  pinMode(HALL_A_PIN, INPUT);
  pinMode(HALL_B_PIN, INPUT);

  delay(100);

  previousState = readHallState();
  hallA = (previousState >> 1) & 1;
  hallB = previousState & 1;
  lastTransitionMicros = micros();

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL_B_PIN), hallISR, CHANGE);

  Serial.println();
  Serial.println(F("SmartRoll SR-HALL-TEST-002"));
  Serial.println(F("Arduino Nano / 2x A3144 Hall test"));
  Serial.println(F("--------------------------------"));
  Serial.println(F("A = D2, B = D3"));
  Serial.println(F("Baud = 115200"));
  Serial.println(F("Initial state A B:"));
  printState(previousState);
  Serial.println();
  Serial.println(F("Commands: s=status, r=reset counters"));
  Serial.println();
}

void printStatus() {
  byte state;
  unsigned long transitions;
  unsigned long invalid;
  unsigned long period;

  noInterrupts();
  state = previousState;
  transitions = transitionCount;
  invalid = invalidCount;
  period = pulsePeriodMicros;
  interrupts();

  Serial.print(F("STATE="));
  printState(state);
  Serial.print(F("  transitions="));
  Serial.print(transitions);
  Serial.print(F("  invalid="));
  Serial.print(invalid);
  Serial.print(F("  last_dt_us="));
  Serial.println(period);
}

void resetCounters() {
  noInterrupts();
  transitionCount = 0;
  invalidCount = 0;
  pulsePeriodMicros = 0;
  lastTransitionMicros = micros();
  interrupts();

  Serial.println(F("Counters reset."));
}

void loop() {
  static byte lastPrintedState = 255;
  static unsigned long lastPrintTime = 0;

  // Print every state change. This makes the raw A/B sequence easy to copy
  // into the test evidence directory.
  byte state;
  unsigned long transitions;
  unsigned long invalid;
  unsigned long period;

  noInterrupts();
  state = previousState;
  transitions = transitionCount;
  invalid = invalidCount;
  period = pulsePeriodMicros;
  interrupts();

  if (state != lastPrintedState) {
    Serial.print(millis());
    Serial.print(F(" ms  A="));
    Serial.print((state >> 1) & 1);
    Serial.print(F(" B="));
    Serial.print(state & 1);
    Serial.print(F("  transitions="));
    Serial.print(transitions);
    Serial.print(F("  invalid="));
    Serial.print(invalid);
    Serial.print(F("  dt_us="));
    Serial.println(period);
    lastPrintedState = state;
    lastPrintTime = millis();
  }

  // Optional periodic status line during a stationary test.
  if (millis() - lastPrintTime > 2000) {
    printStatus();
    lastPrintTime = millis();
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 's' || c == 'S') {
      printStatus();
    } else if (c == 'r' || c == 'R') {
      resetCounters();
    }
  }
}
