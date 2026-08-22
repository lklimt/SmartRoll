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

  Experimentally verified transitions:

    10 -> 11 = CW
    11 -> 01 = CW
    01 -> 11 = CCW
    11 -> 10 = CCW

  IMPORTANT:
  This is NOT a generic quadrature decoder. The state machine is
  deliberately based on the measured SmartRoll geometry.

  After startup/restart:
    - current A/B state is read
    - direction starts as UNKNOWN
    - the first valid transition establishes direction

  Invalid transitions are reported and are NOT counted as movement.
*/

const byte HALL_A_PIN = 2;
const byte HALL_B_PIN = 3;

// Direction values
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

void printStatus() {
  Serial.print("state=");
  Serial.print(stateName(previousState));
  Serial.print(" direction=");
  Serial.print(directionName(direction));
  Serial.print(" CW=");
  Serial.print(cwSteps);
  Serial.print(" CCW=");
  Serial.print(ccwSteps);
  Serial.print(" invalid=");
  Serial.println(invalidTransitions);
}

void processTransition(byte oldState, byte newState) {
  int transitionDirection = DIR_UNKNOWN;

  // Verified SmartRoll transitions for the 30-degree test geometry.
  if (oldState == 2 && newState == 3) {          // 10 -> 11
    transitionDirection = DIR_CW;
  }
  else if (oldState == 3 && newState == 1) {     // 11 -> 01
    transitionDirection = DIR_CW;
  }
  else if (oldState == 1 && newState == 3) {     // 01 -> 11
    transitionDirection = DIR_CCW;
  }
  else if (oldState == 3 && newState == 2) {     // 11 -> 10
    transitionDirection = DIR_CCW;
  }

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

  if (transitionDirection == DIR_CW) {
    cwSteps++;
  } else {
    ccwSteps++;
  }

  // A valid transition tells us the physical direction after restart.
  direction = transitionDirection;

  Serial.print("  ");
  Serial.print(directionName(transitionDirection));
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
  Serial.println("========================================");
}

void loop() {
  byte currentState = readHallState();

  if (currentState != previousState) {
    processTransition(previousState, currentState);
    previousState = currentState;
  }

  // Periodic status message makes it easy to see that the controller
  // remains alive even when the rotor is stationary.
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
