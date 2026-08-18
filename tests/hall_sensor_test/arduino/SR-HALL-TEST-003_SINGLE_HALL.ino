/*
  SmartRoll – SR-HALL-TEST-003_SINGLE_HALL
  Arduino Nano / ATmega328P
  SINGLE HALL SENSOR TEST

  Hardware:
    Hall A OUT -> D2
    Hall A VCC -> 5V
    Hall A GND -> GND
    10k pull-up -> between Hall A OUT and +5V
    100nF -> between Hall A VCC and GND

  Hall B / D3 is NOT USED in this test.
  Do not connect D3 to anything.

  Serial: 115200 baud

  Purpose:
    1. Verify one A3144 Hall sensor.
    2. Observe its switching state.
    3. Count transitions.
    4. Measure time between transitions.

  IMPORTANT:
    The previous two-Hall firmware monitored D3 with an interrupt.
    An unconnected D3 is a floating input and can therefore generate
    random transitions. This single-Hall firmware does not touch D3.
*/

const byte HALL_A_PIN = 2;

volatile byte hallAState = 0;
volatile unsigned long transitionCount = 0;
volatile unsigned long lastTransitionMicros = 0;
volatile unsigned long transitionPeriodMicros = 0;

void hallA_ISR() {
  byte newState = digitalRead(HALL_A_PIN) ? 1 : 0;
  unsigned long now = micros();

  transitionPeriodMicros = now - lastTransitionMicros;
  lastTransitionMicros = now;

  if (newState != hallAState) {
    hallAState = newState;
    transitionCount++;
  }
}

void printStatus() {
  byte state;
  unsigned long transitions;
  unsigned long period;

  noInterrupts();
  state = hallAState;
  transitions = transitionCount;
  period = transitionPeriodMicros;
  interrupts();

  Serial.print(F("A="));
  Serial.print(state);
  Serial.print(F("  transitions="));
  Serial.print(transitions);
  Serial.print(F("  dt_us="));
  Serial.println(period);
}

void resetCounters() {
  noInterrupts();
  transitionCount = 0;
  transitionPeriodMicros = 0;
  lastTransitionMicros = micros();
  interrupts();

  Serial.println(F("Counters reset."));
}

void setup() {
  Serial.begin(115200);

  // External 10k pull-up is used according to the wiring document.
  pinMode(HALL_A_PIN, INPUT);

  delay(100);

  hallAState = digitalRead(HALL_A_PIN) ? 1 : 0;
  lastTransitionMicros = micros();

  attachInterrupt(digitalPinToInterrupt(HALL_A_PIN), hallA_ISR, CHANGE);

  Serial.println();
  Serial.println(F("SmartRoll SR-HALL-TEST-003_SINGLE_HALL"));
  Serial.println(F("Arduino Nano / A3144 single Hall test"));
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Hall A OUT = D2"));
  Serial.println(F("Hall B / D3 = NOT USED"));
  Serial.println(F("Serial = 115200 baud"));
  Serial.println(F("Initial A state = "));
  Serial.println(hallAState);
  Serial.println();
  Serial.println(F("Commands: s=status, r=reset counters"));
  Serial.println();
}

void loop() {
  static byte lastPrintedState = 255;
  static unsigned long lastStatusTime = 0;

  byte state;
  unsigned long transitions;
  unsigned long period;

  noInterrupts();
  state = hallAState;
  transitions = transitionCount;
  period = transitionPeriodMicros;
  interrupts();

  // Print immediately when Hall A changes state.
  if (state != lastPrintedState) {
    Serial.print(millis());
    Serial.print(F(" ms  A="));
    Serial.print(state);
    Serial.print(F("  transitions="));
    Serial.print(transitions);
    Serial.print(F("  dt_us="));
    Serial.println(period);

    lastPrintedState = state;
    lastStatusTime = millis();
  }

  // Periodic status while stationary.
  if (millis() - lastStatusTime >= 2000) {
    printStatus();
    lastStatusTime = millis();
  }

  if (Serial.available()) {
    char c = Serial.read();

    if (c == 's' || c == 'S') {
      printStatus();
    }
    else if (c == 'r' || c == 'R') {
      resetCounters();
    }
  }
}
