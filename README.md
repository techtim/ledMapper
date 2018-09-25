<p align="center">
<img alt="ledMapper icon" src="https://github.com/techtim/ledMapper/blob/master/images/ledMapper_icon_200.png" width="200"></p>

#ledMapper

Open-source application for receiving input from Syphon/Spout or built-in video player and mapping it to distributed ligting system based on network connected RaspberryPi clients serving as controllers for digital LED ICs like LPD8806, WS281X, SK9822, SK6822, etc.

<p align="center">
<img alt="ledMapper screenshot" src="https://github.com/techtim/ledMapper/blob/master/images/ledMapper_screenshot.png" width="800"></p>

Compiled Apps for Windows and OSX and image for RaspberryPi 3 can be found here: https://yadi.sk/d/wtCvC4sx3Ndqwv
*NOTE: RPI Image is for Version 0.1 isn't ready, current image compatible with Alpha executables. For 0.1 lmListener needs to be installed.*

consists:

- app for mapping/grabbing LEDS from Syphon (OSX) or Spout(WIN) or built-in video player (ver 0.1)
- executables on RPI side, listens for udp packets and send via GPIO data to LEDs

###Raspberry Pi 

C++ based UDP listener, listen localhost:3001 and sending data to GPIO pins. (https://github.com/techtim/lmListener)

*NOTE: In Alpha version: 3000 port - for DATA+CLOCK ICs (SK9822...), 3001 - for DATA only ICs WS281X, SK6812*

Pin numbers depends on LED IC type:
LPD8806/SK9822 - SCK > GPIO11 (SPI_CLK), DATA > GPIO10 (SPI_MOSI) (code based on https://github.com/eranrund/blinky-pants/tree/master/lpd)
WS281X driver from https://github.com/jgarff/rpi_ws281x using 2 channel PWM on GPIO12 and GPIO13

<img alt="RPI LED connection scheme" src="https://github.com/techtim/ledMapper/blob/master/images/RPI_3_ledMapper_pinout.png" width="420">

RPI1/2 LPD8806 - tested
RPI3 SK9822 (BLUE PINS)
RPI2/3 WS2812 - tested (GREEN PINS)

For more precious editing use XML files
- WIN C:\Users\Public\Documents\LedMapper
- MAC /Users/Shared/LedMapper

When app has started run Resolume, add Syphon/Spout Server and have fun.
