/*

MadLed Protocol - Written by MadNinja/James Johnston
Assisted by FanMan03 and Legion

Copyright 2020 James Johnston

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <FastLED.h>
#include <EEPROM.h>

class LEDDevice {
  public:
    uint8_t numberOfLeds;
    byte pin;
    String name;
    /* CRGB* leds;
      void setLED(int led, int r, int g, int b)
      {
       CRGB ledcolor;
       ledcolor.setRGB(r, g, b);

       leds[led] = ledcolor;
      }

      CRGB getLED(int led)
      {
       return leds[led];
      }*/
};

#include "HID-Project.h"

#define DP1 1
#define DP2 2
#define DP3 3
#define DP4 4
#define DP5 5
#define DP6 6
#define DP7 7
#define DP8 8
uint8_t gBrightness = 192;
LEDDevice ledDevices[8];

const int pinLed = LED_BUILTIN;
const int pinButton = 2;

uint8_t rawhidData[64];


unsigned int bitOut(void)
{
  static unsigned long firstTime = 1, prev = 0;
  unsigned long bit1 = 0, bit0 = 0, x = 0, limit = 99;
  if (firstTime)
  {
    firstTime = 0;
    prev = analogRead(A0);
  }
  while (limit--)
  {
    x = analogRead(A0);
    bit1 = (prev != x ? 1 : 0);
    prev = x;
    x = analogRead(A0);
    bit0 = (prev != x ? 1 : 0);
    prev = x;
    if (bit1 != bit0)
      break;
  }
  return bit1;
}
//------------------------------------------------------------------------------
unsigned long seedOut(unsigned int noOfBits)
{
  // return value with 'noOfBits' random bits set
  unsigned long seed = 0;
  for (int i = 0; i < noOfBits; ++i)
    seed = (seed << 1) | bitOut();
  return seed;
}

CRGB* leds;
int mxleds = 1;
CLEDController *controllers[8];
void setup() {

  //Serial.begin(115200);
  leds = new CRGB[1];
  // Set the RawHID OUT report array.
  // Feature reports are also (parallel) possible, see the other example for this.
  delay(5000);

  /*FastLED.addLeds<WS2812B, DP1>(leds, intNoOfLed);
    FastLED.addLeds<WS2812B, DP2>(leds, intNoOfLed);
    FastLED.addLeds<WS2812B, DP3>(leds, intNoOfLed);
    FastLED.addLeds<WS2812B, DP4>(leds, intNoOfLed);
    FastLED.addLeds<WS2812B, DP5>(leds, intNoOfLed);
    FastLED.addLeds<WS2812B, DP6>(leds, intNoOfLed);
    FastLED.addLeds<WS2812B, DP7>(leds, intNoOfLed);
    FastLED.addLeds<WS2812B, DP8>(leds, intNoOfLed);*/
  //////Serial.println("Checking for config...");
  char sID[4];
  for (int i = 0; i < 4; i++) {
    sID[i] = EEPROM.read(i);
  }

  if (sID[0] == 77 && sID[1] == 76 && sID[2] == 78 && sID[3] == 71) {

    ////Serial.println("Existing config found...");

  } else {
    ////Serial.println("no config found...creating");
    ////Serial.println(sID[0]);
    ////Serial.println(sID[1]);
    ////Serial.println(sID[2]);
    ////Serial.println(sID[3]);
    unsigned long seed = seedOut(31);
    randomSeed(seed);

    char key[4] = "MLNG";

    char eeprom[40];
    for (int i = 0; i < 20; i++) {
      eeprom[i] = " ";
    }

    for (int i = 0; i < 4; i++) {
      eeprom[i] = key[i];
    }


    for (int p = 0; p < 20; p++) {
      char xx = char(random(65, 90));
      if (random(0, 10) > 7) {
        xx = char(random(48, 57));
      }

      int dst = 4 + p;
      eeprom[dst] = xx;
    }

    for (int i = 0; i < 20; i++) {
      EEPROM.write(i, eeprom[i]);
      eeprom[i + 20] = " ";
    }


    //////Serial.println("Generated //Serial:");
    ////Serial.println(eeprom);
    //////Serial.println("End");
  }

  delay(500);

  RawHID.begin(rawhidData, sizeof(rawhidData));

}

uint8_t reportData[64];

void setupDevice(byte intDataPin, uint8_t intNoOfLed) {
  ledDevices[intDataPin - 1].numberOfLeds = intNoOfLed;
  //ledDevices[intDataPin - 1].leds = new CRGB[intNoOfLed];

  if (intNoOfLed > mxleds) {

    mxleds = intNoOfLed;
    leds = new CRGB[intNoOfLed];
  }

  switch (intDataPin)
  {

    case 1:
      controllers[0] = &FastLED.addLeds<WS2812B, DP1>(leds, intNoOfLed);
      break;

    case 2:
      controllers[1] = &FastLED.addLeds<WS2812B, DP2>(leds, intNoOfLed);
      break;

    case 3:
      controllers[2] = &FastLED.addLeds<WS2812B, DP3>(leds, intNoOfLed);
      break;

    case 4:
      controllers[3] = &FastLED.addLeds<WS2812B, DP4>(leds, intNoOfLed);
      break;

    case 5:
      controllers[4] = &FastLED.addLeds<WS2812B, DP5>(leds, intNoOfLed);
      break;

    case 6:
      controllers[5] = &FastLED.addLeds<WS2812B, DP6>(leds, intNoOfLed);
      break;

    case 7:
      controllers[6] = &FastLED.addLeds<WS2812B, DP7>(leds, intNoOfLed);
      break;

    case 8:
      controllers[7] = &FastLED.addLeds<WS2812B, DP8>(leds, intNoOfLed);
      break;
  }

  for (int f = 0; f < intNoOfLed; f++)
  {
    leds[f].setRGB(255, 0, 0);
    //ledDevices[].setLED(f, 255, 0, 0);
  }
  controllers[intDataPin - 1]->showLeds(gBrightness);
  //FastLED.show();
  delay(100);

  for (int f = 0; f < intNoOfLed; f++)
  {
    leds[f].setRGB( 0, 255, 0);
  }
  //FastLED.show();
  controllers[intDataPin - 1]->showLeds(gBrightness);
  delay(100);

  for (int f = 0; f < intNoOfLed; f++)
  {
    leds[f].setRGB(0, 0, 255);
  }

  controllers[intDataPin - 1]->showLeds(gBrightness);
  delay(100);

  for (int f = 0; f < intNoOfLed; f++)
  {
    leds[f].setRGB(0, 0, 0);
  }
  controllers[intDataPin - 1]->showLeds(gBrightness);
}

int packetsReceived = 0;
void loop() {
  // Check if there is new data from the RawHID device
  uint8_t bytesAvailable = RawHID.available();
  if (bytesAvailable)
  {
    //Serial.print("Bytes availabe: ");
    //Serial.println(bytesAvailable);
    int ct = 0;
    bool anydata = false;
    while (bytesAvailable--)
    {
      reportData[ct] = RawHID.read();
      if (reportData[ct] > 0) {
        anydata = true;
      }
      ct++;
    }

    if (anydata) {
      packetsReceived++;
      bool doneCommand = false;

      uint8_t cmd = reportData[0];

      if (cmd == 0 || cmd == 10)
      {
        byte intDataPin = reportData[1];
        uint8_t intNoOfLed = reportData[2];
        uint8_t deviceclass = reportData[3];

        char name[16];
        for (int i = 0; i < 16; i++) {
          name[i] = reportData[4 + i];
        }

        setupDevice(intDataPin, intNoOfLed);
        reportData[0] = 128;
        reportData[63] = 127;
        int pos = (intDataPin) * 20;

        if (cmd == 10)
        {
          for (int i = 0; i < 20; i++) {
            EEPROM.write(pos + i, reportData[i]);
          }
        }

        reportData[0] = 128;
        RawHID.write(reportData, sizeof(reportData));
      }



      if (cmd == 1)
      {
        uint32_t convertedCol;
        //Serial.println("cmd 1 received... setting rgb");
        byte intDataPin = reportData[1];
        byte page = reportData[2];
        int ct = 19 * page;
        int cp = 3;
        int i = 0;
        while (cp < 63) {

          //todo - omg this is so messy.
          //im just converting 16bit 565 RGB to normal 8 bit RGB.

          byte lb = reportData[cp + 0];
          byte hb = reportData[cp + 1];
          //byte b = reportData[cp + 2];
          uint16_t color = (hb * 256) + lb;
          uint32_t bits = (uint32_t)color;
          uint32_t blue = bits & 0x001F;     // 5 bits blue
          uint32_t green = bits & 0x07E0;    // 6 bits green
          uint32_t red = bits & 0xF800;      // 5 bits red
          convertedCol = (red << 8) | (green << 5) | (blue << 3) | 0xFF000000;
          uint8_t *ccbytes = (uint8_t *)&convertedCol;

          if (ct < ledDevices[intDataPin - 1].numberOfLeds) {

            leds[ct].setRGB(ccbytes[1], ccbytes[0], ccbytes[2]);

          }

          ct++;
          cp = cp + 2;
          i++;
        }

        controllers[intDataPin - 1]->showLeds(gBrightness);
        //FastLED.show();
      }

      if (cmd == 2)
      {
        //Serial.println("cmd 2 received... sending pin data");

        byte intDataPin = reportData[1];
        int pos = (intDataPin) * 20;
        for (int i = 0; i < 64; i++) {
          reportData[i] = 0;
        }

        for (int i = 0; i < 20; i++) {
          reportData[i] = EEPROM.read(i + pos);
        }

        reportData[63] = 127;
        RawHID.write(reportData, sizeof(reportData));
      }

//This is obsolete now
/*      if (cmd == 3)
      {
         FastLED.show();
      }*/

      if (cmd == 5)
      {
        //Serial.println("cmd 5 received... sending eeprom data");
        for (int i = 0; i < 64; i++) {
          reportData[i] = 0;
        }

        for (int i = 0; i < 20; i++) {
          reportData[i] = EEPROM.read(i );
        }
        reportData[63] = 127;
        RawHID.write(reportData, sizeof(reportData));
      }
    }
  }
}
