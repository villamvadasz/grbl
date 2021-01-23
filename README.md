# grbl

The Project is not finished yet. Still under work...

The Project contains a port of GRBL from Atmel to a PIC32MX and/or PIC32MZ. 
Supposed to run on the boards from Olimex:
- PIC32-PINGUINO-MICRO (can be used for testing without any CNC machine or stepper motors)
- PIC32-PINGUINO-OTG (CNC shields fit here very nice)
- PIC32-HMZ144 @200MHz :)
However either a CNC shield is needed or the HW in the hardware section of this project.

Features:
- Faster CPU @200MHz/@80MHz -> More CPU time is left for other usefull upcoming features
- More Flash and RAM -> More space for new features
- GRBL code adapted so that much of the synchronous calls are now asynchronous (probe, home, dwell and everything else) -> Easyier integration with other softwarecomponents like FAT32, SPI, AES, USB CDC driver or whatever component that might be used in future.
- Only soft reset is provided, hard reset is removed. -> Sorry but other components might be not happy to be reseted just like so
- Added Eeprom Emulation -> Position save into Data Flash. Since flash is used it only stores changes incrementally, keeps flash long lifed
- USB CDC communication -> No external device is necessary to connect to the PC.
- Ethernet UDP communication
- Unexpected resets now does not lost machine position to an accuracy of +-1 Step

## Variants
Currently there are several variants available. 
	- Ethernet + USB: Real + Simulator
	- Ethernet UDP: Not maintained
	- USB: Not maintained

### usb_ethernet_tcp
This is the variant that is being activly developed! Use this!
	Makefile_440F256H (for PIC32-PINGUINO-MICRO and Hardware): To build the Real one with a real configuration for the hardware present in the HW section this file is needed
	Makefile_440F256H_simulator (for PIC32-PINGUINO-MICRO and Hardware) : To build a simulator that can be connected and does like a real CNC but does not read in any signals, nor generates them. Ok it is generating them into a dummy variable.
You can connect over Ethernet on Port 5123
	Also availalbe to different CPUs:
	Makefile_460F512L
	Makefile_470F512H 
	Makefile_470F512L
	The 200MHz variant is currently under massive rework:
	Makefile_32MZ2048ECG144 (for PIC32-HMZ144): Not working yet.
#### Building the project
Need: XC32 2.30 compiler Full Version.
build_all.bat will build all bootloaders, bootloader updated and application.
build.bat will build application.
#### Flash Bootloader
Need: Mplab 8.92 with PICkit3 (other tools are also good like Mplab X, Mplab IPE, ICD3, ICD4)
Choose either bootloader_ethernet_PIC or bootloader_PIC (for USB) bootloader. Sadly there is no bootloader that support booth channels. Preferred to use Ethernet!
Open the project. 
Select debugger tool.
Optionally change IP address in TCPIPConfig.h. There is no DHCP supported.
Build. 
Flash.
Start bootloader_PC. 
Select USB or Ethernet. 
Add IP address.
Connect.
Select HEX File for flashing: Most probably it will cnc_1_0_0_440F256H.hex or cnc_1_0_0_440F256H_simulator.hex
Erase-Program-Verfy
Run Application
Disconnect
Close Bootloader

To use the board you need a special version of UGS. Somehow the original does not communicate properly with my board. I have already sent my findigs to them, so hopefully the will fix it. Until then use this one:
https://github.com/villamvadasz/Universal-G-Code-Sender
	
### ethernet_udp
Not maintained. Too old. Use the one with TCP instead!
The problem with UDP is that packets might get lost. So after a while it can happen that the file is sent completly, but UDS is still waiting for OK responses. Since it does not send 1 command and waits for Ok. It sends more than one command and then waits for more then one ok.
This variant is being used on my actual CNC machine.

### usb_cdc
Not maintained. Too old. Use the one with ethernet and usb. Should be the same as this.
This variant was used for my CNC machine. Not using any more since USB does not offer galvanic isolation by default. Note that, if you are using USB, always by a DCDC Galvanic isolation for USB on ebay (the cheap stuff is ok) and add some chockes on the USB cable booth end.
	- Makefile_440F256H_arduino 
	- Makefile_440F256H_old_grbl 

## Tools
make: Everything is built from build.bat with a make.
bootloader_PC: Bootloader. Can be entered by either doing a Reset or Power On (waits some seconds in the bootloader), or sending BOOTLOADER command in UGS.
netcat: Can be used to send a reset packet. See batch files. Sadly not completly automated. Still need some work, but it works.
disoverer_pc: Sends an UPD broadcast, that will be answered by the device. This way it is earyier to get the IP address.
eeprom_analyser:

## Configure project
Please see the excel table, which is used to generate the configuration for GRBL.

# Note
In some files the Copyright might be deleted by accident. Sorry... But mostly you can assume that either it was the original GRBL author or Microchip.

