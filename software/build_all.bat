@echo off
cls
call build.bat

rem cd .\bootloader_ethernet_PIC
rem call build.bat
rem cd ..

cd .\bootloader_PIC
call build.bat
cd ..

cd .\bootloader_updater_ethernet_PIC
call build.bat
cd ..

cd .\bootloader_updater_PIC
call build.bat
cd ..
