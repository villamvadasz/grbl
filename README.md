# grbl

The Project is not finished yet. Still under work...

The Project contains a port of GRBL from Atmel to a PIC32MX and/or PIC32MZ. 
Supposed to run on the boards from Olimex:
- PIC32-PINGUINO-MICRO (can be used for testing without any CNC machine or stepper motors)
- PIC32-PINGUINO-OTG (CNC shields fit here very nice)
- PIC32-HMZ144 @200MHz :)

Features:
- Faster CPU @200MHz -> More CPU time is left for other usefull upcoming features
- More Flash and RAM -> More space for new features
- GRBL code adapted so that much of the synchronous calls are now asynchronous (probe, home, dwell and everything else) -> Easyier integration with other softwarecomponents like FAT32, SPI, AES, USB CDC driver or whatever component that might be used in future.
- Only soft reset is provided, hard reset is removed. -> Sorry but other components might be not happy to be reseted just like so
- Added Eeprom Emulation -> Position save into Data Flash. Since flash is used it only stores changes incrementally, keeps flash long lifed
- USB CDC communication -> No external device is necessary to connect to the PC.
- Ethernet UDP communication

## Variants
Currently there are two variants available. Ethernet and USB.

### ethernet_udp
This variant was used for testing the ethernet communication. So it is not even configured really.

### usb_cdc
This variant was used for my CNC machine.

## Configure project
Please see the excel table, which is used to generate the configuration for GRBL.

# Note
In some files the Copyright might be deleted by accident. Sorry...

