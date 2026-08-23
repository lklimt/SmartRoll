/*
  SmartRoll - Hall quadrature direction test
  SR-HALL-TEST-005_QUADRATURE

  DEVELOPMENT TEST ONLY
  Hardware: Arduino Nano / ATmega328P

  Hall A -> D2
  Hall B -> D3

  Verified mechanical arrangement:
    - 2 magnets, 180 degrees apart
    - Hall A and Hall B mounted directly beside each other
    - nominal air gap: 5 mm

  MEASURED SEQUENCES

  Clockwise:
    11 -> 10 -> 00 -> 01 -> 11

  Counter-clockwise:
    11 -> 01 -> 00 -> 10 -> 11

  This is a real 2-bit quadrature encoder sequence.

  Direction is determined from the COMPLETE state transition:

    CW:
      11 -> 10
      10 -> 00
      00 -> 01
      01 -> 11

    CCW:
      11 -> 01
      01 -> 00
      00 -> 10
      10 -> 11

  Any other transition is invalid and is not counted.

  IMPORTANT STARTUP BEHAVIOUR
  ---------------------------
  After reset/power failure the current Hall state is sampled and stored.
  Direction is set to UNKNOWN. The next valid transition determines the
  direction immediately, even if the rotor is already moving.

  The algorithm does NOT assume a previous direction survives a restart.
*/

const byte HALL_A_PIN = 2;
const byte HALL_B_PIN = 3;

const int DIR_UNKNOWN = 0;
const int DIR_CW      = 1;
const int DIR_CCW     = -1;

byte previousState = 0;
int direction = DIR_UNKNOWN;

long cwTransitions = 0;
long ccwTransitions = 0;
long invalidTransitions = 0;
long totalValidTransitions = 0;

unsigned long lastStatus = 0;

byte readHallState() {
  // State encoding:
  // A=0 B=0 -> 00 -> 0
  // A=0 B=1 -> 01 -> 1
  // A=1 B=0 -> 10 -> 2
  // A=1 B=1 -> 11 -> 3
  return (digitalRead(HALL_A_PIN) << 1) | digitalRead(HALL_B_PIN);
}

const char* stateName(byte state) {
  switch (state) {
    case 0: return "00";
    case 1: return "01";
    case 2: return "10";
    case 3: return "11";
    default: return "??";
  }
}

const char* directionName(int dir) {
  if (dir == DIR_CW)  return "CW";
  if (dir == DIR_CCW) return "CCW";
  return "UNKNOWN";
}

int decodeTransition(byte oldState, byte newState) {
  // CW sequence: 11 -> 10 -> 00 -> 01 -> 11
  if ((oldState == 3 && newState == 2) ||
      (oldState == 2 && newState == 0) ||
      (oldState == 0 && newState == 1) ||
      (oldState == 1 && newState == 3)) {
    return DIR_CW;
  }

  // CCW sequence: 11 -> 01 -> 00 -> 10 -> 11
  if ((oldState == 3 && newState == 1) ||
      (oldState == 1 && newState == 0) ||
      (oldState == 0 && newState == 2) ||
      (oldState == 2 && newState == 3)) {
    return DIR_CCW;
  }

  return DIR_UNKNOWN;
}

void processTransition(byte oldState, byte newState) {
  int transitionDirection = decodeTransition(oldState, newState);

  Serial.print(stateName(oldState));
  Serial.print(" -> ");
  Serial.print(stateName(newState));

  if (transitionDirection == DIR_UNKNOWN) {
    invalidTransitions++;

    Serial.print("  INVALID");
    Serial.print("  invalid=");
    Serial.println(invalidTransitions);
    return;
  }

  totalValidTransitions++;
  direction = transitionDirection;

  if (direction == DIR_CW) {
    cwTransitions++;
  } else {
    ccwTransitions++;
  }

  Serial.print("  direction=");
  Serial.print(directionName(direction));
  Serial.print("  steps(CW/CCW)=");
  Serial.print(cwTransitions);
  Serial.print("/");
  Serial.print(ccwTransitions);
  Serial.print("  invalid=");
  Serial.println(invalidTransitions);
}

void setup() {
  // INPUT_PULLUP is compatible with the current Hall test wiring.
  pinMode(HALL_A_PIN, INPUT_PULLUP);
  pinMode(HALL_B_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  delay(300);

  // Critical restart behaviour:
  // remember only the current electrical state, NOT the old direction.
  previousState = readHallState();
  direction = DIR_UNKNOWN;

  Serial.println();
  Serial.println("========================================");
  Serial.println("SmartRoll Hall Quadrature Test 005");
  Serial.println("Arduino Nano / ATmega328P");
  Serial.println("Hall A = D2   Hall B = D3");
  Serial.println("========================================");
  Serial.println("CW : 11 -> 10 -> 00 -> 01 -> 11");
  Serial.println("CCW: 11 -> 01 -> 00 -> 10 -> 11");
  Serial.println();

  Serial.print("Initial state: ");
  Serial.println(stateName(previousState));
  Serial.println("Initial direction: UNKNOWN");
  Serial.println();
}

void loop() {
  byte currentState = readHallState();

  if (currentState != previousState) {
    processTransition(previousState, currentState);
    previousState = currentState;
  }

  if (millis() - lastStatus >= 1000) {
    lastStatus = millis();

    Serial.print("STATUS state=");
    Serial.print(stateName(previousState));
    Serial.print(" direction=");
    Serial.print(directionName(direction));
    Serial.print(" valid=");
    Serial.print(totalValidTransitions);
    Serial.print(" invalid=");
    Serial.println(invalidTransitions);
  }
}
