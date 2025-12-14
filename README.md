# grbl

The Project is not finished yet. Still under work... Lot of the stuffs might be here even outdated.

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
- Ethernet TCP communication
- Unexpected resets now does not lost machine position to an accuracy of +-1 Step

## Variants
There are no variants now. Former UDP variant is deleted, since it is not really usefull due to lack of transport protocol. USB and Ethernet can work in the same Software together, so there is no need to separate it.

## User Manual
### USB/TCP Ethernet
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
Need: XC32 4.20 compiler Full Version.
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
Add serial number to the label box and click the button.
Add the crypto key to the label box and click the button.
Select HEX File for flashing: Most probably it will cnc_1_0_0_440F256H.hex or cnc_1_0_0_440F256H_simulator.hex
Add the signature from the signature_bootloader.txt to the corresponding device and hex file.
Erase-Program-Verfy
Run Application
Disconnect
Close Bootloader

To use the board the UGS is higly suggested:
https://winder.github.io/ugs_website/
	
## Tools
make: Everything is built from build.bat with a make.
bootloader_PC: Bootloader. Can be entered by either doing a Reset or Power On (waits some seconds in the bootloader), or sending BOOTLOADER command in UGS.
netcat: Can be used to send a reset packet. See batch files. Sadly not completly automated. Still need some work, but it works.
disoverer_pc: Sends an UPD broadcast, that will be answered by the device. This way it is earyier to get the IP address.
eeprom_analyser:

## Configure project
Please see the excel table, which is used to generate the configuration for GRBL.

# Safety Concept
The emergency stop or abort is sensitiv on booth edges (rising and falling). This way, any change on the line will cause an abort. Not only pressing the button, but releasing it too. In case somehow the signal would be inverted, the emergency stop or abort would still work.

The input of the abort is not invertable in SW. It is assumed that:
//uC PIN is 1 ==> Emergency abort button is pressed in, so machine must stop everything
//uC PIN is 0 ==> Emergency abort button is released, it is allowed to move

Stepper enable signal gets disabled after 50ms. This window can be used to slow down the machine (not implemented yet). After 50ms the signal will be set to disable the motors.

Stepper motor enable works only if the function properly calculates some multiplications. This way it must be called with the proper parameter. If by mistake the code is jumped on, there is less likly to enable the stepper motors, since the calculation will fail.

# Note
In some files the Copyright might be deleted by accident. Sorry... But mostly you can assume that either it was the original GRBL author or Microchip.

