ledMapper
=========

App for receiving Syphon input and mapping it to LPD8806/WS2812 LED strips

consists:

- app for mapping/grabbing LEDS from Syphon input (OSX for now)
- executables on RPI side, listens for udp packets and send via GPIO data to LEDs

Raspberry Pi 

C based UDP listeners, listen localhost:3000 and sending data to GPIO pins.
Pin numbers depends on LED IC type:
LPD8806 - SCK > GPIO11 (SPI_CLK), DATA > GPIO10 (SPI_MOSI) (code based on https://github.com/eranrund/blinky-pants/tree/master/lpd)
WS2812 - DATA > GPIO18 (code based on https://github.com/jgarff/rpi_ws281x)

RPI1/2 LPD8806 - tested

RPI2 WS2812 - tested

Steps:

- copy executable files to your /home/pi/udpWS281x
- type '
	cd ~
	chmod ugo+x udpWS281x udpLPD8806
	'
- if you using LPD8806 activate SPI in RPI config, type 'sudo raspi-config'
- try 'sudo ./udpWS281x' or 'sudo ./LPD8806'
- folowing this manual https://www.raspberrypi.org/documentation/linux/usage/rc-local.md add to rc.local :
'/home/pi/udpWS281x &' or '/home/pi/udpLPD8806 &' for now they working on the same port so choose


OSX

LedMapper - app on openFrameworks (when not "Show Controllers" left/right arrows switch active controller)
tested on OS X 10.9.5

Kind of software manual:

	- Click on controller to activate / ESC to deactivate all 
	- CMD+Click - add line points in active controller / SHIFT+Click add circle of points 
	- BKSPS+Click - delete clicked point 
	- 's' - save , 'l' - load 
	- When turn off 'Show Controlles' you can switch between all controlles to show individual maps using LEFT/RIGHT keys";
	- 'h' - show help

For more precious editing use XML files in data/Ctrls0.

After app run Resolume, add Syphon Server and have fun.
