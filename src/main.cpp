#include <Arduino.h>
#include <MeAuriga.h>
#include <SoftwareSerial.h>

#define FORWARD 1
#define REVERSE 2
#define LEFT 3
#define RIGHT 4
#define MLEFT 5
#define MRIGHT 6
#define STOP 7

MeRGBLed rgbled_0(0, 12);
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
void isr_process_encoder1(void);
void isr_process_encoder2(void);
void moveDuration(float seconds);
void move(int direction, int speed);

// Helper function to extract int/float after a keyword
String extractValue(String cmd, String key) {
    int idx = cmd.indexOf(key);
    if (idx == -1) return "";
    
    // Start after the key and any whitespace
    int start = idx + key.length();
    while (start < cmd.length() && isSpace(cmd.charAt(start))) {
        start++;
    }
    
    // Find end (next space or end of string)
    int end = start;
    while (end < cmd.length() && !isSpace(cmd.charAt(end))) {
        end++;
    }
    
    String value = cmd.substring(start, end);
    Serial.print("Extracted '"); Serial.print(value); 
    Serial.print("' from key '"); Serial.print(key); Serial.println("'");
    
    return value;
}

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
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);

  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);

  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
}

void loop()
{

  if (Serial.available() > 0)
  {
    delay(50); // Add a short delay to allow full message to arrive
    String command = Serial.readStringUntil('\n');
command.trim(); // Remove leading/trailing whitespace and newlines
    Serial.flush();
    Serial.println("Received:");
    Serial.println(command);
    rgbled_0.setColor(3, 0, 0, 0);
    rgbled_0.setColor(12, 0, 0, 0);
    rgbled_0.setColor(6, 0, 0, 0);
    rgbled_0.show();

    // Check for complex command
    if (command.startsWith("left:"))
    {
        int leftVal = 0, rightVal = 0;
        float durationVal = 0.0;

        int leftIdx = command.indexOf("left:");
        int rightIdx = command.indexOf("right:");
        int durIdx = command.indexOf("duration:");

        if (leftIdx != -1 && rightIdx != -1 && durIdx != -1)
        {
            Serial.println("Raw command: [" + command + "]");
            
            leftVal = extractValue(command, "left:").toInt();
            rightVal = extractValue(command, "right:").toInt();
            durationVal = extractValue(command, "duration:").toFloat();
            
            Serial.println("Values after conversion:");
            Serial.print("leftVal: "); Serial.println(leftVal);
            Serial.print("rightVal: "); Serial.println(rightVal);
            Serial.print("durationVal: "); Serial.println(durationVal);

            rgbled_0.setColor(3, 255, 255, 0);
            rgbled_0.setColor(6, 255, 255, 0);
            rgbled_0.show();

            Encoder_1.setTarPWM(leftVal);
            Encoder_2.setTarPWM(rightVal);
            moveDuration(durationVal);

            // Always stop motors after moveDuration
            Encoder_1.setTarPWM(0);
            Encoder_2.setTarPWM(0);
            Encoder_1.loop();
            Encoder_2.loop();

            rgbled_0.setColor(3, 0, 255, 0);
            rgbled_0.setColor(6, 0, 255, 0);
            rgbled_0.show();
        }
        else
        {
            Serial.println("Invalid complex command format.");
        }
    }
    else if (command == "f\n")
    {
      rgbled_0.setColor(3, 0, 255, 0);
      rgbled_0.show();
      move(FORWARD, 100);
      moveDuration(1.5);
    }
    else if (command == "b\n")
    {
      rgbled_0.setColor(3, 255, 0, 0);
      rgbled_0.show();
      move(REVERSE, 100);
      moveDuration(0.5);
    }
    else if (command == "l\n")
    {
      rgbled_0.setColor(3, 0, 255, 0);
      rgbled_0.setColor(12, 0, 255, 0);
      rgbled_0.show();
      move(LEFT, 50);
      moveDuration(0.5);
    }
    else if (command == "r\n")
    {
      rgbled_0.setColor(3, 0, 255, 0);
      rgbled_0.setColor(6, 0, 255, 0);
      rgbled_0.show();
      move(RIGHT, 50);
      moveDuration(0.5);
    }
    else if (command == "mr\n")
    {
      rgbled_0.setColor(3, 0, 255, 0);
      rgbled_0.setColor(6, 0, 255, 255);
      rgbled_0.show();
      move(MRIGHT, 100);
      moveDuration(0.5);
    }
    else if (command == "ml\n")
    {
      rgbled_0.setColor(3, 0, 255, 0);
      rgbled_0.setColor(12, 0, 255, 255);
      rgbled_0.show();
      move(MLEFT, 100);
      moveDuration(0.5);
    }
    else if (command == "s\n")
    {
      rgbled_0.setColor(3, 255, 0, 0);
      rgbled_0.setColor(12, 255, 0, 0);
      rgbled_0.setColor(6, 255, 0, 0);
      rgbled_0.show();

      move(STOP, 100);
      moveDuration(0);
    }
    else
    {
      rgbled_0.setColor(2, 255, 0, 0);

      rgbled_0.show();
      move(STOP, 100);
      moveDuration(0);  
    }
  }
  else
  {
    rgbled_0.setColor(2, 255, 255, 255);

    rgbled_0.show();

    move(STOP, 100);
    moveDuration(0);
  }
}

void isr_process_encoder1(void)
{
  if (digitalRead(Encoder_1.getPortB()) == 0)
  {
    Encoder_1.pulsePosMinus();
  }
  else
  {
    Encoder_1.pulsePosPlus();
    ;
  }
}
void isr_process_encoder2(void)
{
  if (digitalRead(Encoder_2.getPortB()) == 0)
  {
    Encoder_2.pulsePosMinus();
  }
  else
  {
    Encoder_2.pulsePosPlus();
  }
}
void move(int direction, int speed)
{
  int leftSpeed = 0;
  int rightSpeed = 0;
  if (direction == FORWARD)
  {
    leftSpeed = -speed;
    rightSpeed = speed;
  }
  else if (direction == REVERSE)
  {
    leftSpeed = speed;
    rightSpeed = -speed;
  }
  else if (direction == LEFT)
  {
    leftSpeed = -speed;
    rightSpeed = -speed;
  }
  else if (direction == RIGHT)
  {
    leftSpeed = speed; 
    rightSpeed = speed;
  }
  else if (direction == MLEFT)
  {
    leftSpeed = -speed;
    rightSpeed = speed / 1.5;
  }
  else if (direction == MRIGHT)
  {
    leftSpeed = -speed / 1.5;
    rightSpeed = speed;
  }
  else if (direction == STOP)
  {
    leftSpeed = 0;
    rightSpeed = 0;
  }
  Encoder_1.setTarPWM(leftSpeed);
  Encoder_2.setTarPWM(rightSpeed);
}

void moveDuration(float seconds)
{
  if (seconds < 0.0)
  {
    seconds = 0.0;
  }
  unsigned long endTime = millis() + seconds * 1000;
  int i = 0;
  while (millis() < endTime)
  {
    i+=1;
    if (i % 1000 == 0){
      Serial.print("millis:");
      Serial.println(millis());
    }
    Encoder_1.loop();
    Encoder_2.loop();
  }

  Encoder_1.setTarPWM(0);
  Encoder_2.setTarPWM(0);

  Encoder_1.loop();
  Encoder_2.loop();
}
// runPackage(5,_util.short2array(leftSpeed),_util.short2array(rightSpeed));