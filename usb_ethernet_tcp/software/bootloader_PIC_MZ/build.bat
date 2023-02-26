@echo off
cls

del /q .\output\*.hex
del /q .\output\*.d
del /q .\output\*.o
del /q .\output\*.map

set TARGET_FILE_NAME=bootloader_PIC_MZ

set CPU_TARGET=PIC32MZ2048EFM100
set SYS_TARGET=200MHZ
call :makecall_internal

set CPU_TARGET=PIC32MZ2048EFM100
set XTAL_TARGET=24MHZ
set SYS_TARGET=200MHZ
call :makecall_external


rem set CPU_TARGET=PIC32MZ2048ECG144
rem set SYS_TARGET=200MHZ
rem call :makecall_internal

set CPU_TARGET=PIC32MZ2048ECG144
set XTAL_TARGET=12MHZ
set SYS_TARGET=200MHZ
call :makecall_external



goto :end


:makecall_external
del /q .\output\build_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET% clean
.\make\make.exe -f Makefile_%CPU_TARGET% all -j8 CFLAGS="-DOSC_USER_DEFINE -DXTAL_%XTAL_TARGET% -DSYS_FREQ_%SYS_TARGET%"
copy .\output\build.hex .\output\%TARGET_FILE_NAME%_%CPU_TARGET%_Q%XTAL_TARGET%_F%SYS_TARGET%.hex
copy .\output\build.map .\output\%TARGET_FILE_NAME%_%CPU_TARGET%_Q%XTAL_TARGET%_F%SYS_TARGET%.map
.\make\make.exe -f Makefile_%CPU_TARGET% clean
del /q .\output\build.map
del /q .\output\build.map
goto :eof

:makecall_internal
del /q .\output\build_%CPU_TARGET%.hex
.\make\make.exe -f Makefile_%CPU_TARGET% clean
.\make\make.exe -f Makefile_%CPU_TARGET% all -j8 CFLAGS="-DOSC_USER_DEFINE -DINT_OSC_8MHZ_EC_24 -DSYS_FREQ_%SYS_TARGET%"
copy .\output\build.hex .\output\%TARGET_FILE_NAME%_%CPU_TARGET%_RCINT8_F%SYS_TARGET%.hex
copy .\output\build.map .\output\%TARGET_FILE_NAME%_%CPU_TARGET%_RCINT8_F%SYS_TARGET%.map
.\make\make.exe -f Makefile_%CPU_TARGET% clean
del /q .\output\build.map
del /q .\output\build.map
goto :eof

:end
del /q .\output\*.d

