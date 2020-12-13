#include <FastLED.h>

class LEDDevice {
  public:
    uint8_t numberOfLeds;
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

#include "HID-Project.h"

#define DP1 1
#define DP2 2
#define DP3 3
#define DP4 4
#define DP5 5
#define DP6 6
#define DP7 7
#define DP8 8

LEDDevice ledDevices[8];

const int pinLed = LED_BUILTIN;
const int pinButton = 2;

uint8_t rawhidData[64];

void setup() {

  //Serial.begin(115200);

  // Set the RawHID OUT report array.
  // Feature reports are also (parallel) possible, see the other example for this.
  delay(500);

  RawHID.begin(rawhidData, sizeof(rawhidData));
}

uint8_t reportData[64];

void setupDevice(byte intDataPin, uint8_t intNoOfLed) {
  ledDevices[intDataPin - 1].numberOfLeds = intNoOfLed;
  ledDevices[intDataPin - 1].leds = new CRGB[intNoOfLed];
  switch (intDataPin)
  {

    case 1:
            FastLED.addLeds<WS2812B, DP1>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;

    case 2:
            FastLED.addLeds<WS2812B, DP2>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;

    case 3:
            FastLED.addLeds<WS2812B, DP3>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;

    case 4:
            FastLED.addLeds<WS2812B, DP4>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;

    case 5:
            FastLED.addLeds<WS2812B, DP5>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;

    case 6:
            FastLED.addLeds<WS2812B, DP6>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;

    case 7:
            FastLED.addLeds<WS2812B, DP7>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;

    case 8:
            FastLED.addLeds<WS2812B, DP8>(ledDevices[intDataPin - 1].leds, intNoOfLed);
      break;
  }

  for (int f = 0; f < intNoOfLed; f++)
  {
    ledDevices[intDataPin - 1].setLED(f, 255, 0, 0);
  }
  FastLED.show();
  delay(500);

  for (int f = 0; f < intNoOfLed; f++)
  {
    ledDevices[intDataPin - 1].setLED(f, 0, 255, 0);
  }
  FastLED.show();
  delay(500);

  for (int f = 0; f < intNoOfLed; f++)
  {
    ledDevices[intDataPin - 1].setLED(f, 0, 0, 255);
  }
  FastLED.show();
  delay(500);

  for (int f = 0; f < intNoOfLed; f++)
  {
    ledDevices[intDataPin - 1].setLED(f, 0, 0, 0);
  }
  FastLED.show();
  delay(500);
}

void loop() {
  // Check if there is new data from the RawHID device
  uint8_t bytesAvailable = RawHID.available();
  if (bytesAvailable)
  {
    int ct = 0;
    bool anydata = false;
    while (bytesAvailable--)
    {
      reportData[ct] = RawHID.read();
      if (reportData[ct] > 0 && ct > 0) {
        anydata = true;
      }
      ct++;
    }

    if (anydata) {
      char cmd = reportData[0];

      if (cmd == 0)
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

        RawHID.write(reportData, sizeof(reportData));
      }

      if (cmd == 1)
      {
        byte intDataPin = reportData[1];
        byte page = reportData[2];
        int ct = 19 * page;
        int cp = 3;
        for (int i = 0; i < 19; i++) {
          byte r = reportData[cp + 0];
          byte g = reportData[cp + 1];
          byte b = reportData[cp + 2];

          if (ct < ledDevices[intDataPin - 1].numberOfLeds) {

            ledDevices[intDataPin - 1].setLED(ct, r, g, b);

          }

          ct++;
          cp = cp + 3;

        }

      }

      if (cmd == 2)
      {
  //todo
      }

      if (cmd == 3)
      {
        FastLED.show();
      }

    }
  }
}
