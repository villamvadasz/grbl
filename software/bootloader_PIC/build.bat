@echo off
cls

del /q .\output\*.hex
del /q .\output\*.d
del /q .\output\*.o
del /q .\output\*.map


set CURDIR=%CD%
set TEMP=%CURDIR%\output
set TMP=%CURDIR%\output


set CPU_TARGET=440F256H
set SYS_TARGET=80MHZ
call :makecall_internal
set CPU_TARGET=470F512H
set SYS_TARGET=80MHZ
call :makecall_internal
set CPU_TARGET=460F512L
set SYS_TARGET=80MHZ
call :makecall_internal
set CPU_TARGET=470F512L
set SYS_TARGET=80MHZ
call :makecall_internal
set CPU_TARGET=795F512H
set SYS_TARGET=80MHZ
call :makecall_internal

set CPU_TARGET=795F512H
set SYS_TARGET=80MHZ
call :makecall_internal_sdcard

set CPU_TARGET=440F256H
set SYS_TARGET=80MHZ
call :makecall_internal_eth
set CPU_TARGET=470F512H
set SYS_TARGET=80MHZ
call :makecall_internal_eth
set CPU_TARGET=460F512L
set SYS_TARGET=80MHZ
call :makecall_internal_eth
set CPU_TARGET=470F512L
set SYS_TARGET=80MHZ
call :makecall_internal_eth
set CPU_TARGET=795F512H
set SYS_TARGET=80MHZ
call :makecall_internal_eth

set CPU_TARGET=795F512H
set SYS_TARGET=80MHZ
call :makecall_internal_eth_decrypt


set CPU_TARGET=795F512H
set SYS_TARGET=80MHZ
call :makecall_internal_sdcard_usb



goto :end


:makecall_external
del /q .\output\USBHIDBootloader_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET% clean
.\make\make.exe -f Makefile_%CPU_TARGET% all -j8 CFLAGS="-DOSC_USER_DEFINE -DXTAL_%XTAL_TARGET% -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader.hex .\output\USBHIDBootloader_%CPU_TARGET%_Q%XTAL_TARGET%_F%SYS_TARGET%.hex
copy .\output\USBHIDBootloader.map .\output\USBHIDBootloader_%CPU_TARGET%_Q%XTAL_TARGET%_F%SYS_TARGET%.map
.\make\make.exe -f Makefile_%CPU_TARGET% clean
del /q .\output\USBHIDBootloader.map
del /q .\output\USBHIDBootloader.map
goto :eof

:makecall_internal
del /q .\output\USBHIDBootloader_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET% clean
.\make\make.exe -f Makefile_%CPU_TARGET% all -j8 CFLAGS="-DOSC_USER_DEFINE -DXTAL_8MHZ -DINT_OSC_8MHZ -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader.hex .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%.hex
copy .\output\USBHIDBootloader.map .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%.map
.\make\make.exe -f Makefile_%CPU_TARGET% clean
del /q .\output\USBHIDBootloader.map
del /q .\output\USBHIDBootloader.map
goto :eof

:makecall_internal_sdcard
del /q .\output\USBHIDBootloader_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET%_sdcard clean
.\make\make.exe -f Makefile_%CPU_TARGET%_sdcard all -j8 CFLAGS="-DTRANSPORT_LAYER_SD_CARD -DOSC_USER_DEFINE -DXTAL_8MHZ -DINT_OSC_8MHZ -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader.hex .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_sdcard.hex
copy .\output\USBHIDBootloader.map .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_sdcard.map
.\make\make.exe -f Makefile_%CPU_TARGET%_sdcard clean
del /q .\output\USBHIDBootloader.map
del /q .\output\USBHIDBootloader.map
goto :eof


:makecall_internal_eth
del /q .\output\USBHIDBootloader_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET%_eth clean
.\make\make.exe -f Makefile_%CPU_TARGET%_eth all -j8 CFLAGS="-DTRANSPORT_LAYER_ETH -DOSC_USER_DEFINE -DXTAL_8MHZ -DINT_OSC_8MHZ -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader.hex .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_eth.hex
copy .\output\USBHIDBootloader.map .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_eth.map
.\make\make.exe -f Makefile_%CPU_TARGET%_eth clean
del /q .\output\USBHIDBootloader.map
del /q .\output\USBHIDBootloader.map
goto :eof

:makecall_internal_eth_decrypt
del /q .\output\USBHIDBootloader_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET%_eth clean
.\make\make.exe -f Makefile_%CPU_TARGET%_eth all -j8 CFLAGS="-DBOOTLOADER_DECRYPT -DTRANSPORT_LAYER_ETH -DOSC_USER_DEFINE -DXTAL_8MHZ -DINT_OSC_8MHZ -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader.hex .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_eth_decrypt.hex
copy .\output\USBHIDBootloader.map .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_eth_decrypt.map
.\make\make.exe -f Makefile_%CPU_TARGET%_eth clean
del /q .\output\USBHIDBootloader.map
del /q .\output\USBHIDBootloader.map
goto :eof

:makecall_internal_sdcard_usb
del /q .\output\USBHIDBootloader_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET%_sdcard clean
.\make\make.exe -f Makefile_%CPU_TARGET%_sdcard all -j8 CFLAGS="-DBOOTLOADER_DECRYPT -DTRANSPORT_LAYER_USB -DTRANSPORT_LAYER_SD_CARD -DOSC_USER_DEFINE -DXTAL_8MHZ -DINT_OSC_8MHZ -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader.hex .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_sdcard_usb.hex
copy .\output\USBHIDBootloader.map .\output\USBHIDBootloader_%CPU_TARGET%_RCINT8_F%SYS_TARGET%_sdcard_usb.map
.\make\make.exe -f Makefile_%CPU_TARGET%_sdcard clean
del /q .\output\USBHIDBootloader.map
del /q .\output\USBHIDBootloader.map
goto :eof


:end
del /q .\output\*.d

