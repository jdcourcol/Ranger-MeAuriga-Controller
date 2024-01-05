#include <Arduino.h>
#include <MeAuriga.h>
#include <SoftwareSerial.h>

MeRGBLed rgbled_0(0, 12);

void setup()
{
  // See line 130 of ../lib/Makeblock-Libraries-3.26/examples/Firmware_for_Auriga/Firmware_for_Auriga.ino
  // #define RGBLED_PORT                          44
  // RGBLED is on pin 44
  rgbled_0.setpin(44);
  Serial.begin(115200);
  Serial.println("Bluetooth Start!");
  // Turn off all LEDs
  rgbled_0.setColor(0, 0, 0, 0);
  rgbled_0.show();
}

void loop()
{
  if (Serial.available() > 0 )
  {
    rgbled_0.setColor(2, 0, 255, 0);
    rgbled_0.show();
    String command = Serial.readString();
    Serial.flush();
    Serial.println("Received:");
    Serial.println(command); 
    if (command == "a\n") {
      rgbled_0.setColor(3, 0, 0, 255);
      rgbled_0.show();
      delay(200);
    }
    else if (command == "b\n") {
      rgbled_0.setColor(3, 255, 255, 0);
      rgbled_0.show();
      delay(200);
    }
  }
  else {
    rgbled_0.setColor(2, 255, 255, 255);

    rgbled_0.show();

  }
}