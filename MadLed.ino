#include <FastLED.h>

class LEDDevice{
  public:
    int numberOfLeds;
    byte pin;
    String name;
    CRGB* leds;
    void setLED(int led, int r, int g, int b)
    {
      CRGB ledcolor;
      ledcolor.setRGB(r,g,b);

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

// Define the array of leds

LEDDevice ledDevices[8];

void setup() { 
Serial.begin(115200);
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
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

char rx_byte = 0;
String rx_str = "";

void loop() { 

  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    
    if (rx_byte != '\n') {
      // a character of the string was received
      rx_str += rx_byte;
    }
    else {
      // end of string
Serial.println(">"+rx_str);
String cmd = getValue(rx_str,':',0);
if (cmd == ""){
cmd=rx_str;
  
}

if (cmd=="ping"){

  Serial.println("pong");
}

if (cmd=="help"){

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
}

if (cmd=="set"){

String cmdBank = getValue(rx_str,':',1);
String cmdLed = getValue(rx_str,':',2);
String cmdR = getValue(rx_str,':',3);
String cmdG = getValue(rx_str,':',4);
String cmdB = getValue(rx_str,':',5);

int intBank = cmdBank.toInt();
int intLed = cmdLed.toInt();
int intR = cmdR.toInt();
int intG = cmdG.toInt();
int intB = cmdB.toInt();

ledDevices[intBank].setLED(intLed,intR,intG,intB);
  FastLED.show();
  
}


if (cmd=="get"){

String cmdBank = getValue(rx_str,':',1);
String cmdLed = getValue(rx_str,':',2);

int intBank = cmdBank.toInt();
int intLed = cmdLed.toInt();

CRGB cl = ledDevices[intBank].getLED(intLed);

Serial.print(cl.r);
Serial.print(":");
Serial.print(cl.g);
Serial.print(":");
  Serial.println(cl.b);
  
}

if (cmd=="add"){
String cmdBank = getValue(rx_str,':',1);
int intBank = cmdBank.toInt();



String cmdDataPin = getValue(rx_str,':',2);
int intDataPin = cmdDataPin.toInt();

String cmdNoOfLed = getValue(rx_str,':',3);
int intNoOfLed = cmdNoOfLed.toInt();

String cmdName = getValue(rx_str,':',4);

ledDevices[intBank].name = cmdName;
ledDevices[intBank].numberOfLeds = intNoOfLed;
ledDevices[intBank].leds = new CRGB[intNoOfLed];
switch (intDataPin){
  
case 1:
 FastLED.addLeds<NEOPIXEL, DP1>(ledDevices[intBank].leds, intNoOfLed);  
break;

case 2:
 FastLED.addLeds<NEOPIXEL, DP2>(ledDevices[intBank].leds, intNoOfLed);  
break;

case 3:
 FastLED.addLeds<NEOPIXEL, DP3>(ledDevices[intBank].leds, intNoOfLed);  
break;


}

}

if (cmd=="devices")
{

for(int i=0;i<3;i++){
   Serial.print(i);
   Serial.print(":");
   Serial.print(ledDevices[i].numberOfLeds);
  Serial.println(":"+ledDevices[i].name);
}
  
}

      
      rx_str = "";                // clear the string for reuse


  
      
      
    }
  } // end: if (Serial.available() > 0)





  
  
}
