ledMapper
=========

App for receiving Syphon input and mapping it to LPD8806/WS2812 LED strips using network connected RaspberryPi clients.

Compiled Apps for Windows and OSX and image for RaspberryPi 3 can be found here: https://yadi.sk/d/wtCvC4sx3Ndqwv

consists:

- app for mapping/grabbing LEDS from Syphon (OSX) or Spout(WIN) input
- executables on RPI side, listens for udp packets and send via GPIO data to LEDs

Raspberry Pi 

C based UDP listeners, listen localhost:3000 and sending data to GPIO pins.
Pin numbers depends on LED IC type:
LPD8806/SK9822 - SCK > GPIO11 (SPI_CLK), DATA > GPIO10 (SPI_MOSI) (code based on https://github.com/eranrund/blinky-pants/tree/master/lpd)

<img alt="RPI LED connection scheme" src="https://github.com/techtim/ledMapper/blob/master/RPI_3_ledMapper_pinout.png" width="420">

RPI1/2 LPD8806 - tested
RPI3 SK9822 (BLUE PINS)
RPI2/3 WS2812 - tested (GREEN PINS)

Steps:

- copy executable files to your /home/pi/udpWS281x
- type '
	cd ~ && chmod ugo+x udpWS281x udpLPD8806
	'
- if you using LPD8806 activate SPI in RPI config, type 'sudo raspi-config'
- try 'sudo ./udpWS281x' or 'sudo ./LPD8806'
- folowing this manual https://www.raspberrypi.org/documentation/linux/usage/rc-local.md add to rc.local :
'/home/pi/udpWS281x &' or '/home/pi/udpLPD8806 &' for now they working on the same port so choose


OSX:

LedMapper -  app made with openFrameworks tested on OS X and Windows VS12 with oF tag 0.9.8 

Drag Syphon.framework file from ofxSyphon folder into Xcode ledMapper project source tree (select copy if needed and add targets).

WIN:

Read how to install ofxSpout 

Kind of software manual:

	- Click on controller to activate / ESC to deactivate all 
	- Hold "1"/"2"/"3"+Click - add line/circle/matrix grabbing object of pixels in active controller
	- BKSPS+Click on grabbing object - delete it 
	- 's' - save , 'l' - load 
	- When turn off 'Show Controlles' you can switch between all controlles to show individual maps using LEFT/RIGHT keys";
	- 'h' - show help

For more precious editing use XML files
- WIN C:\Users\Public\Documents\LedMapper
- MAC /Users/Shared/LedMapper

When app has started run Resolume, add Syphon/Spout Server and have fun.
