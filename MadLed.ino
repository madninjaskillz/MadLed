#include <FastLED.h>

class LEDDevice {
  public:
    int numberOfLeds;
    byte pin;
    String name;
    CRGB* leds;
    void setLED(int led, int r, int g, int b)
    {
      CRGB ledcolor;
      ledcolor.setRGB(r, g, b);

      leds[led] = ledcolor;
    }

    CRGB getLED(int led)
    {
      return leds[led];
    }
};

#define DP1 1
#define DP2 2
#define DP3 3
#define DP4 4
#define DP5 5
#define DP6 6
#define DP7 7
#define DP8 8

char rx_byte = 0;
String rx_str = "";
String tx_str = "";
bool showRequired = false;
LEDDevice ledDevices[8];

void setup() {
  Serial.begin(19200);
  Serial.println("MadLed by Mad Ninja / James Johnston 2020");
  Serial.println("-----------------------------------------");
  Serial.println("https://github.com/madninjaskillz/MadLed");
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void sprintln(String input)
{
  tx_str = tx_str + input + "\r\n";
}

void sprint(String input)
{
  tx_str = tx_str + input;
}

void processCommand(String input)
{
  //sprintln(">"+input);
  String cmd = getValue(rx_str, ':', 0);
  if (cmd == "")
  {
    cmd = rx_str;
  }

  if (cmd == "ping")
  {
    sprintln("pong");
  }

  if (cmd == "ning")
  {
    sprintln("nong");
  }

  /*
    if (cmd == "help") {

      Serial.println("MadLed by Mad Ninja / James Johnston 2020");
      Serial.println("-----------------------------------------");
      Serial.println("https://github.com/madninjaskillz/MadLed");
      Serial.println("add:{bank}:{datapin}:{ledCount}:[name]");
      Serial.println("add a fan in bank [bank] on datapin [datapin] with [ledCount] leds called '[name]'");
      Serial.println("  bank: bank/device number from 0 to 7");
      Serial.println("  datapin: which arduino pin is the fan data line connected to (1 to 8)");
      Serial.println("  ledCount: how many LEDs does the fan have?");
      Serial.println("  name: Friendly name");
      Serial.println("eg: add:0:3:21:Top Front");
      Serial.println("");
      Serial.println("set:[bank]:[led]:[Red]:[Green]:[Blue]");
      Serial.println("set led [led] on device [bank] to R/G/B");
      Serial.println("  bank: bank/device number from 0 to 7");
      Serial.println("  led: led number");
      Serial.println("  Red/Green/Blue: value from 0 to 255");
      Serial.println("eg: set:0:15:255:0:255");
      Serial.println("  ");
      Serial.println("get:[bank]:[led]");
      Serial.println("Get the rgb in format RRR:GGG:BBB of led [led] on device [bank]");
      Serial.println("  bank: bank/device number from 0 to 7");
      Serial.println("  led: led number");
      Serial.println("eg: get:0:15");
    }*/

  if (cmd == "set")
  {

    String cmdBank = getValue(input, ':', 1);
    String cmdLed = getValue(input, ':', 2);
    String cmdR = getValue(input, ':', 3);
    String cmdG = getValue(input, ':', 4);
    String cmdB = getValue(input, ':', 5);

    int intBank = cmdBank.toInt();
    int intLed = cmdLed.toInt();
    int intR = cmdR.toInt();
    int intG = cmdG.toInt();
    int intB = cmdB.toInt();

    ledDevices[intBank].setLED(intLed, intR, intG, intB);
    showRequired = true;
  }

  if (cmd == "bulk")
  {

    String cmdBank = getValue(input, ':', 1);
    String cmdLed = getValue(input, ':', 2);
    String cmdR = getValue(input, ':', 3);
    String cmdG = getValue(input, ':', 4);
    String cmdB = getValue(input, ':', 5);

    String cmdLed1 = getValue(cmdLed, '-', 0);
    String cmdLed2 = getValue(cmdLed, '-', 1);

    int intBank = cmdBank.toInt();
    int intLed1 = cmdLed1.toInt();
    int intLed2 = cmdLed2.toInt();
    int intR = cmdR.toInt();
    int intG = cmdG.toInt();
    int intB = cmdB.toInt();

    for (int i = intLed1; i <= intLed2; i++)
    {
      ledDevices[intBank].setLED(i, intR, intG, intB);
    }

    showRequired = true;
  }

  if (cmd == "get")
  {

    String cmdBank = getValue(input, ':', 1);
    String cmdLed = getValue(input, ':', 2);

    int intBank = cmdBank.toInt();
    int intLed = cmdLed.toInt();

    CRGB cl = ledDevices[intBank].getLED(intLed);

    sprint(cl.r + "");
    sprint(":");
    sprint(cl.g + "");
    sprint(":");
    sprintln(cl.b + "");
  }

  if (cmd == "add")
  {
    String cmdBank = getValue(input, ':', 1);
    int intBank = cmdBank.toInt();

    String cmdDataPin = getValue(input, ':', 2);
    int intDataPin = cmdDataPin.toInt();

    String cmdNoOfLed = getValue(input, ':', 3);
    int intNoOfLed = cmdNoOfLed.toInt();

    String cmdName = getValue(input, ':', 4);

    ledDevices[intBank].name = cmdName;
    ledDevices[intBank].numberOfLeds = intNoOfLed;
    ledDevices[intBank].leds = new CRGB[intNoOfLed];
    switch (intDataPin)
    {

      case 1:
        FastLED.addLeds<NEOPIXEL, DP1>(ledDevices[intBank].leds, intNoOfLed);
        break;

      case 2:
        FastLED.addLeds<NEOPIXEL, DP2>(ledDevices[intBank].leds, intNoOfLed);
        break;

      case 3:
        FastLED.addLeds<NEOPIXEL, DP3>(ledDevices[intBank].leds, intNoOfLed);
        break;

      case 4:
        FastLED.addLeds<NEOPIXEL, DP4>(ledDevices[intBank].leds, intNoOfLed);
        break;

      case 5:
        FastLED.addLeds<NEOPIXEL, DP5>(ledDevices[intBank].leds, intNoOfLed);
        break;

      case 6:
        FastLED.addLeds<NEOPIXEL, DP6>(ledDevices[intBank].leds, intNoOfLed);
        break;

      case 7:
        FastLED.addLeds<NEOPIXEL, DP7>(ledDevices[intBank].leds, intNoOfLed);
        break;

      case 8:
        FastLED.addLeds<NEOPIXEL, DP8>(ledDevices[intBank].leds, intNoOfLed);
        break;
    }

    for (int f = 0; f < intNoOfLed; f++)
    {
      ledDevices[intBank].setLED(f, 255, 0, 0);
    }
    FastLED.show();
    delay(500);

    for (int f = 0; f < intNoOfLed; f++)
    {
      ledDevices[intBank].setLED(f, 0, 255, 0);
    }
    FastLED.show();
    delay(500);

    for (int f = 0; f < intNoOfLed; f++)
    {
      ledDevices[intBank].setLED(f, 0, 0, 255);
    }
    FastLED.show();
    delay(500);

    for (int f = 0; f < intNoOfLed; f++)
    {
      ledDevices[intBank].setLED(f, 0, 0, 0);
    }
    FastLED.show();
    delay(500);
  }

  if (cmd == "devices")
  {
    for (int i = 0; i < 8; i++) {
      sprint(i + "");
      sprint(":");
      sprint(ledDevices[i].numberOfLeds + "");
      sprintln(":" + ledDevices[i].name);
    }
  }
}
int arrivingdatabyte = 0;
void loop()
{
  if (Serial.available() > 0)
  {
    arrivingdatabyte = Serial.read( );

    if (arrivingdatabyte == '\n' || arrivingdatabyte == '~') {
      rx_str.trim();
      processCommand(rx_str);
      rx_str = "";
    } else {
      rx_str += (char)arrivingdatabyte;
    }
  }

  if (tx_str.length() > 0) {
    Serial.print(tx_str);
    tx_str = "";

  }


  if (showRequired) {
    FastLED.show();
    showRequired = false;
  }

}
