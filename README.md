# node-rpi-si4703
Node wrapper for the rpi-si4703 library.  For use with the [Si4703 breakout board at sparkfun.](https://www.sparkfun.com/products/12938)
***
## Usage

**Currently you must run as sudo to access the hardware.**

Make sure you have the [wiring pi library](http://wiringpi.com/download-and-install/) installed.

You can use callbacks or you can promisify it with bluebird.  The following example is with callbacks.

```
var FMTuner = require('node-rpi-si4703');

var tuner = new FMTuner(23, 0); //GPIO 23 for reset, and SDA for i2c (GPIO 0)

tuner.powerOn((err) => {
  //Check err for any errors before moving on (err is a string by the way)

  //Set to 103.3 Moody Radio Cleveland
  tuner.setChannel(1033, (err) => {
    //Now turn it up! (Volume range is 0-15)
    tuner.setVolume(15, (err) => {
      //Enjoy your FM radio!
    });
  });
});

```
***
## Methods

All methods have a callback with error as the first parameter and any result as the second parameter.

 * **powerOn** - Turns on radio
 * **powerOff** - Turns off radio
 * **setChannel** - Sets a radio channel, pass a frequency multiplied by 10.  For example 1033 for 103.3 Mhz
 * **getChannel** - Gets the current channel the radio is on multiplied by 10
 * **seekUp** - Seek radio up
 * **seekDown** - Seek radio down
 * **setVolume** - Set a volume between 0-15.
 * **readRDS** - Read the RDS message for a station. Pass the frequency to read multiplied by 10.

