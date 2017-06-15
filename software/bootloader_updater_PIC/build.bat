@echo off
cls

del /q .\output\*.hex
del /q .\output\*.d
del /q .\output\*.o
del /q .\output\*.map


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





rem set CPU_TARGET=440F256H
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=440F256H
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=440F256H
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external
rem set CPU_TARGET=440F256H
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external
rem set CPU_TARGET=440F256H
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=20MHZ
rem call :makecall_external
rem set CPU_TARGET=440F256H
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=20MHZ
rem call :makecall_external
rem 
rem 
rem set CPU_TARGET=470F512H
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=470F512H
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=470F512H
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external
rem set CPU_TARGET=470F512H
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external
rem 
rem set CPU_TARGET=460F512L
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=460F512L
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=460F512L
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external
rem set CPU_TARGET=460F512L
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external
rem 
rem set CPU_TARGET=470F512L
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=470F512L
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=80MHZ
rem call :makecall_external
rem set CPU_TARGET=470F512L
rem set XTAL_TARGET=8MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external
rem set CPU_TARGET=470F512L
rem set XTAL_TARGET=20MHZ
rem set SYS_TARGET=40MHZ
rem call :makecall_external



goto :end


:makecall_external
del /q .\output\USBHIDBootloader_updater_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET% clean
.\make\make.exe -f Makefile_%CPU_TARGET% all -j8 CFLAGS="-DOSC_USER_DEFINE -DXTAL_%XTAL_TARGET% -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader_updater.hex .\output\USBHIDBootloader_updater_%CPU_TARGET%_Q%XTAL_TARGET%_F%SYS_TARGET%.hex
copy .\output\USBHIDBootloader_updater.map .\output\USBHIDBootloader_updater_%CPU_TARGET%_Q%XTAL_TARGET%_F%SYS_TARGET%.map
.\make\make.exe -f Makefile_%CPU_TARGET% clean
del /q .\output\USBHIDBootloader_updater.map
del /q .\output\USBHIDBootloader_updater.map
goto :eof

:makecall_internal
del /q .\output\USBHIDBootloader_updater_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET% clean
.\make\make.exe -f Makefile_%CPU_TARGET% all -j8 CFLAGS="-DOSC_USER_DEFINE -DXTAL_8MHZ -DINT_OSC_8MHZ -DSYS_FREQ_%SYS_TARGET%"
copy .\output\USBHIDBootloader_updater.hex .\output\USBHIDBootloader_updater_%CPU_TARGET%_RCINT8_F%SYS_TARGET%.hex
copy .\output\USBHIDBootloader_updater.map .\output\USBHIDBootloader_updater_%CPU_TARGET%_RCINT8_F%SYS_TARGET%.map
.\make\make.exe -f Makefile_%CPU_TARGET% clean
del /q .\output\USBHIDBootloader_updater.map
del /q .\output\USBHIDBootloader_updater.map
goto :eof

:end
del /q .\output\*.d

