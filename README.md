# grbl

The Project is not finished yet. Still under work... Lot of the stuffs might be here even outdated.

The Project contains a port of GRBL from Atmel to a PIC32MX and/or PIC32MZ. 
Supposed to run on the boards from Olimex:
- PIC32-RETROBSD (PIC32MX795F512H)
- PIC32-PINGUINO-MICRO
- PIC32-PINGUINO-OTG
- PIC32-HMZ144 @200MHz

As the PIC32-PINGUINO-MICRO, PIC32-PINGUINO-OTG and the PIC32-HMZ144 is out of stock, the project is now running on the PIC32-RETROBSD. Olders boards are still supported but not really tested, or features are turned off (only non-GRBL).
However either a CNC shield is needed or the HW in the hardware section of this project.

The PIC32-HMZ144 is not really tested, but it might compile. Also the bootloader might be outdated.

Features:
- Faster CPU @200MHz/@80MHz -> More CPU time is left for other usefull upcoming features
- More Flash and RAM -> More space for new features
- GRBL code adapted so that much of the synchronous calls are now asynchronous (probe, home, dwell and everything else) -> Easyier integration with other softwarecomponents like FAT32, SPI, AES, USB CDC driver or whatever component that might be used in future.
- Only soft reset is provided, hard reset is removed. -> Sorry but other components might be not happy to be reseted just like so
- Added Eeprom Emulation -> Position save into Data Flash. Since flash is used it only stores changes incrementally, keeps flash long lifed
- USB CDC communication -> No external device is necessary to connect to the PC.
- Ethernet TCP communication
- Unexpected resets now does not lost machine position to an accuracy of +-1 Step
- Bootloader support encrypted files and signature

## Variants
There are no variants now. Former UDP variant is deleted, since it is not really usefull due to lack of transport protocol. USB and Ethernet can work in the same Software together, so there is no need to separate it.

## User Manual
### USB/TCP Ethernet
	Makefile_795F512H
	Makefile_795F512H_simulator
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
The bootloader_PIC project supports builds for USB, ethernet and SDCard, however only one can be active. So when building the project it will emitt different hex file. Currently the USBHIDBootloader_795F512H_RCINT8_F80MHZ_eth_decrypt.hex is used.
Open the project USBHIDBootloader.mcp. 
Select debugger tool.
Optionally change IP address in TCPIPConfig.h. There is no DHCP supported.
Build or Import one of the prebuilt files. Highly suggested: USBHIDBootloader_795F512H_RCINT8_F80MHZ_eth_decrypt.hex
Flash.
Start bootloader_PC. 
Select USB or Ethernet. 
Add IP address.
Connect.
Add serial number to the label box and click the button. This will write a serial number into the device.
Add the crypto key to the label box and click the button. This will write a crypto key into the deivce.

#### Bootloader Encryption and Signature
It is highly suggested, that in fazit.txt you add your board. This file is not used activley in any process, but is good for storage.
Format is:
```
Serial number in HEX, Cryptokey for encryption and signature, Not used cryptokey but need to be filled, Some comment for you to know something about the device
```
Here is a simple example for 3 entries:
```
FFFFFFFF FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF	Empty device
12345678 00112233445566778899AABBCCDDEEFF 4dd63fe3a4e2f099f45f57f8ad3ba5c0	Development 1
DE000001 00112233445566778899AABBCCDDEEFF d24e07819a5901d89dca84de98410cf8	Development 2
```

Before you execute
```
release.bat
```
you must make sure that your device is added there.
```
set FILE_NAME=cnc_1_0_0_795F512H_simulator.hex
set AES_KEY_SIGNATURE=05cf0d46621dc94caac88a97579f2c3f
set AES_KEY_ENCRYPT=15d1b10d061031e25100ffd2ca04e8e8
set AES_KEY_ENCRYPT=%AES_KEY_SIGNATURE%
set SERIAL_NUMBER=DE000003
set BOOTLOADER_UID=NA
call :sign_enc_file_bootloader
```
FILE_NAME: Select also which HEX file to process.
AES_KEY_SIGNATURE: Signature and encryption keys are the same. So you only need to set AES_KEY_SIGNATURE properly. 
SERIAL_NUMBER: This is important since each device have its unique cryptokeys, so this will make sure the output file have a different name.
BOOTLOADER_UID: Must be NA, not used here, only for update over the air (under development).

When done properly, this will take the HEX file specificed, and calculate a signature for it and creates an encrypted one. The encrypted one make sure that a 3rd party can not reverse engineer the file. The signature make sure that only a signed file can be flashed.

Some features are stil missing. Like the same key is used for Encryption and Signature. Also the whole thing might be cryptographically weak. It is more like a demonstration.

#### Flash Application with Encryption
The application must be properly build with build.bat. Make sure that for your device a signature is generated. For this please see the signature chapter.
You need a bootloader that does support encryption on the target! (example: USBHIDBootloader_795F512H_RCINT8_F80MHZ_eth_decrypt.hex)
Select ENC File for flashing. For example: cnc_1_0_0_795F512H_simulator.hex_DE000003.hex.enc
Add the signature from the signature_bootloader.txt for the corresponding device and hex file.
Erase-Program-Verfy
Run Application
Disconnect
Close Bootloader

#### Flash Application without Encryption
The application must be properly build with build.bat.
You need a bootloader that does NOT support encryption on the target! (example: USBHIDBootloader_795F512H_RCINT8_F80MHZ_eth.hex or USBHIDBootloader_795F512H_RCINT8_F80MHZ.hex)
Select HEX File for flashing. For example: cnc_1_0_0_795F512H_simulator.hex
Unselect the Enable mark in the Crypto part. This will make sure you are flashing simple hex files.
Erase-Program-Verfy
Run Application
Disconnect
Close Bootloader

#### UGS
To use the board the UGS is higly suggested:
https://winder.github.io/ugs_website/
	
## Tools
make: Everything is built from build.bat with a make.
bootloader_PC: Bootloader. Can be entered by either doing a Reset or Power On (waits some seconds in the bootloader), or sending BOOTLOADER command in UGS.
netcat: Can be used to send a reset packet. See batch files. Sadly not completly automated. Still need some work, but it works.
disoverer_pc: Sends an UPD broadcast, that will be answered by the device. This way it is earyier to get the IP address.
signer_pc: Used for encryption and signature generation.
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

