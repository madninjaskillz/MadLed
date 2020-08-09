# MadLed
Simple LED control for multiple 3 pin RGB fans based on WS281X for Arduino (Uno) Boards. Allows control of up to 8 fans per arduino


## Required
* Arduino (Arduino Uno tested)
* ARGB fan with 3 pin header
* Some way to connect said fan to Arduino

## Wiring

### Standard ARGB Connector

![pinout diagram](https://i.imgur.com/ewv27oJ.png)

### Connecting

Connect the 5v pin to the Arduinos 5v pin
Connect the Ground to the Arduinos Ground pin
Connect the Data to one of the arduinos data pins (between 1 and 8)

Multiple fans can share 5v and ground, must have individual data pins.

For permenant installations, would recommend connecting male connectors to arduino to allow easy connection and disconnection

### Direct Use

When running, MadLed provides a basic "interface" via a 115200 baud COM port.

All commands are : seperated. send "help" for a help message;

```
  MadLed by Mad Ninja / James Johnston 2020
  -----------------------------------------
  https://github.com/madninjaskillz/MadLed
  add:{bank}:{datapin}:{ledCount}:[name]
  add a fan in bank [bank] on datapin [datapin] with [ledCount] leds called '[name]'
    bank: bank/device number from 0 to 7
    datapin: which arduino pin is the fan data line connected to (1 to 8)
    ledCount: how many LEDs does the fan have?
    name: Friendly name
  eg: add:0:3:21:Top Front
  
  set:[bank]:[led]:[Red]:[Green]:[Blue]
  set led [led] on device [bank] to R/G/B
    bank: bank/device number from 0 to 7
    led: led number
    Red/Green/Blue: value from 0 to 255
  eg: set:0:15:255:0:255
    
  get:[bank]:[led]
  Get the rgb in format RRR:GGG:BBB of led [led] on device [bank]
    bank: bank/device number from 0 to 7
    led: led number
  eg: get:0:15
```

### RGB.NET

_Coming Soon_

