/*
  SmartRoll - Hall direction test
  SR-HALL-TEST-004_DIRECTION

  DEVELOPMENT TEST ONLY
  Hardware: Arduino Nano / ATmega328P

  Hall A -> D2
  Hall B -> D3

  Tested geometry:
    - 2 magnets, 180 degrees
    - Hall A/B separation: 30 degrees
    - nominal air gap: 5 mm

  IMPORTANT:
  The measured Hall sequence contains the transitions:

    10 -> 11 -> 01 -> 11 -> 10 -> ...

  and the same four-state cycle is traversed continuously. Therefore
  an individual transition such as 10 -> 11 does NOT by itself contain
  enough information to establish direction after startup.

  Direction is established only by an unambiguous transition from state 11:

    11 -> 01  = CW
    11 -> 10  = CCW

  Once direction is known, 10 -> 11 and 01 -> 11 are accepted only when
  they are consistent with the already established direction.

  This is deliberately a SmartRoll-specific state machine, not a generic
  quadrature decoder.

  After startup/restart:
    - current A/B state is read
    - direction starts as UNKNOWN
    - ambiguous transitions do not invent a direction
    - the first unambiguous transition establishes direction

  Invalid transitions are reported and are NOT counted as movement.
*/

const byte HALL_A_PIN = 2;
const byte HALL_B_PIN = 3;

const int DIR_UNKNOWN = 0;
const int DIR_CW      = 1;
const int DIR_CCW     = -1;

byte previousState = 0;
int direction = DIR_UNKNOWN;

long cwSteps = 0;
long ccwSteps = 0;
long invalidTransitions = 0;
long totalValidTransitions = 0;

unsigned long lastPrint = 0;

byte readHallState() {
  byte a = digitalRead(HALL_A_PIN);
  byte b = digitalRead(HALL_B_PIN);
  return (a << 1) | b;
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

void processTransition(byte oldState, byte newState) {
  bool valid = false;
  int transitionDirection = DIR_UNKNOWN;
  bool directionEstablished = false;

  /*
    The two transitions leaving state 11 are unambiguous:

      11 -> 01 = CW
      11 -> 10 = CCW
  */
  if (oldState == 3 && newState == 1) {
    transitionDirection = DIR_CW;
    valid = true;
    directionEstablished = true;
  }
  else if (oldState == 3 && newState == 2) {
    transitionDirection = DIR_CCW;
    valid = true;
    directionEstablished = true;
  }
  /*
    These transitions return to state 11. They are valid only if they
    agree with the direction already established.

      CW:  10 -> 11
      CCW: 01 -> 11

    If direction is UNKNOWN, they are valid Hall movement but cannot yet
    establish direction. We therefore report them as AMBIGUOUS rather
    than falsely choosing a direction.
  */
  else if (oldState == 2 && newState == 3) {
    if (direction == DIR_CW) {
      transitionDirection = DIR_CW;
      valid = true;
    }
    else if (direction == DIR_UNKNOWN) {
      Serial.print(stateName(oldState));
      Serial.print(" -> ");
      Serial.print(stateName(newState));
      Serial.println("  AMBIGUOUS (direction still UNKNOWN)");
      return;
    }
  }
  else if (oldState == 1 && newState == 3) {
    if (direction == DIR_CCW) {
      transitionDirection = DIR_CCW;
      valid = true;
    }
    else if (direction == DIR_UNKNOWN) {
      Serial.print(stateName(oldState));
      Serial.print(" -> ");
      Serial.print(stateName(newState));
      Serial.println("  AMBIGUOUS (direction still UNKNOWN)");
      return;
    }
  }

  Serial.print(stateName(oldState));
  Serial.print(" -> ");
  Serial.print(stateName(newState));

  if (!valid) {
    invalidTransitions++;
    Serial.print("  INVALID  invalid=");
    Serial.println(invalidTransitions);
    return;
  }

  totalValidTransitions++;

  // An unambiguous transition establishes or changes direction.
  if (directionEstablished) {
    if (direction != transitionDirection && direction != DIR_UNKNOWN) {
      Serial.print("  DIRECTION CHANGE: ");
      Serial.print(directionName(direction));
      Serial.print(" -> ");
      Serial.println(directionName(transitionDirection));
    }
    direction = transitionDirection;
  }

  // Count only transitions consistent with the established direction.
  if (direction == DIR_CW) {
    cwSteps++;
  }
  else if (direction == DIR_CCW) {
    ccwSteps++;
  }

  Serial.print("  direction=");
  Serial.print(directionName(direction));
  Serial.print("  steps(CW/CCW)=");
  Serial.print(cwSteps);
  Serial.print("/");
  Serial.println(ccwSteps);
}

void setup() {
  pinMode(HALL_A_PIN, INPUT_PULLUP);
  pinMode(HALL_B_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  delay(300);

  previousState = readHallState();
  direction = DIR_UNKNOWN;

  Serial.println();
  Serial.println("========================================");
  Serial.println("SmartRoll Hall Direction Test 004");
  Serial.println("Arduino Nano / ATmega328P");
  Serial.println("A=D2  B=D3");
  Serial.println("Startup direction = UNKNOWN");
  Serial.print("Initial Hall state = ");
  Serial.println(stateName(previousState));
  Serial.println("11->01 = CW, 11->10 = CCW");
  Serial.println("========================================");
}

void loop() {
  byte currentState = readHallState();

  if (currentState != previousState) {
    processTransition(previousState, currentState);
    previousState = currentState;
  }

  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

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
