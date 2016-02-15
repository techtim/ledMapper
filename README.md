ledMapper
=========

App for receiving Syphon input and mapping it to LPD8806/WS2812 LED strips

consists:
- app for mapping/grabbing LEDS from Syphon input (OSX for now)
- executables on RPI side, listens for udp packets and send via GPIO data to LEDs

OSX
LedMapper - app on openFrameworks (when not "Show Controllers" left/right arrows switch active controller)

Raspberry Pi 
C based UDP listeners, listen localhost:3000 and sending data to GPIO pins.
Pin numbers depends on LED IC type:
LPD8806 - SCK > GPIO11 (SPI_CLK), DATA > GPIO10 (SPI_MOSI)
WS2812 - DATA > GPIO18 (code based on https://github.com/jgarff/rpi_ws281x)
