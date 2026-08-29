/*
  SmartRoll - TEST 3
  ESP32 + ERTE FT45E RF

  Purpose:
    Verify that the RF protocol used by ControlRoll can be generated
    directly from ESP32.

  RF transmitter:
    DATA -> GPIO25
    GND  -> ESP32 GND
    VCC  -> module-rated supply

  Required library:
    RF433send by Sebastien Millet
    Tested architecture support includes ESP32.

  Serial commands at 115200 Bd:
    01C = roller 1 UP
    01B = roller 1 STOP
    01A = roller 1 DOWN

  ERTE payloads:
    UP   = 33 63 13 79 08
    STOP = 33 63 13 79 2A
    DOWN = 33 63 13 79 19

  The RF timing configuration is copied from the proven ControlRoll
  implementation and must not be changed during this test.

  IMPORTANT:
    This is an isolated RF test. Hall sensors, Wi-Fi and other sensors
    are intentionally not used.
*/

#include <Arduino.h>
#include <RF433send.h>

#define PIN_RFOUT 25

RfSend *tx_erte;

const byte data_1nahoru[] = {0x33, 0x63, 0x13, 0x79, 0x08};
const byte data_1stop[]   = {0x33, 0x63, 0x13, 0x79, 0x2A};
const byte data_1dolu[]   = {0x33, 0x63, 0x13, 0x79, 0x19};

void setup()
{
  pinMode(PIN_RFOUT, OUTPUT);
  digitalWrite(PIN_RFOUT, LOW);

  Serial.begin(115200);
  delay(500);

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
      9360,   // sep
      39      // nb_bits / protocol parameter from ControlRoll
  );

  Serial.println();
  Serial.println("========================================");
  Serial.println("SmartRoll TEST 3");
  Serial.println("ESP32 + ERTE FT45E RF");
  Serial.println("========================================");
  Serial.println("RF OUT: GPIO25");
  Serial.println();
  Serial.println("Commands:");
  Serial.println("01C = UP");
  Serial.println("01B = STOP");
  Serial.println("01A = DOWN");
  Serial.println();
  Serial.println("RF433send TRIBIT / ControlRoll timing loaded.");
  Serial.println("Ready.");
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

void loop()
{
  if (!Serial.available()) {
    return;
  }

  String command = Serial.readStringUntil('\n');
  command.trim();

  if (command == "01C") {
    sendCommand("UP", "01C",
                data_1nahoru, sizeof(data_1nahoru));
  }
  else if (command == "01B") {
    sendCommand("STOP", "01B",
                data_1stop, sizeof(data_1stop));
  }
  else if (command == "01A") {
    sendCommand("DOWN", "01A",
                data_1dolu, sizeof(data_1dolu));
  }
  else if (command.length() > 0) {
    Serial.print("Unknown command: ");
    Serial.println(command);
  }
}
