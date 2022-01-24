<p align="center">
<img alt="ledMapper icon" src="https://github.com/techtim/ledMapper/blob/master/images/ledMapper_icon_200.png" width="200"></p>

ledMapper
===

Open-source application for receiving input from Syphon/Spout or built-in video player and mapping it to distributed ligting system based on network connected RaspberryPi clients serving as controllers for digital LED ICs like LPD8806, WS281X, SK9822, SK6822, etc.

<p align="center">
<img alt="ledMapper screenshot" src="https://github.com/techtim/ledMapper/blob/master/images/ledMapper_screenshot.png" width="800"></p>

Check [Interface Description](https://github.com/techtim/ledMapper/wiki/ledMapper-Interface) page for details.

Try out compiled apps for Windows and OSX and image for RaspberryPi 3 on [TVL website](https://tvl.io/soft/ledMapper/)

### Raspberry Pi 

C++ based UDP listener, listen localhost:3001 and sending data to GPIO pins. (https://github.com/techtim/lmListener)

Pin numbers depends on LED IC type:
- LPD8806/SK9822 - SCK > GPIO11 (SPI_CLK), DATA > GPIO10 (SPI_MOSI)
- WS281X driver from https://github.com/jgarff/rpi_ws281x using 2 channel PWM on GPIO12 and GPIO13

<img alt="RPI LED connection scheme" src="https://github.com/techtim/ledMapper/blob/master/images/RPI_3_ledMapper_pinout.png" width="420">

__RPI2/3:__ SK9822/APA102/LPD8806 (BLUE PINS)
__RPI2/3:__ WS2812 (GREEN PINS)

## Build
### Linux
```
git submodule update --init --recursive
rm -rf addons/ofxDatGui/src/libs/ofxSmartFont/example-ofxSmartFont
sudo ./scripts/linux/ubuntu/install_dependencies.sh
cd ../../../apps/projectGenerator/commandLine && make -j4
```
run ProjectGenerator for ledMapper

### Windows
Use VS 2019 with v142
In ofxLedMapper in EASTL submodule update its submodule `git submodule update --init test/package/EABase`