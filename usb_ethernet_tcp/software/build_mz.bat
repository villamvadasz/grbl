@echo off
cls
cd doc
call copy_files.bat
cd ..
del /q .\output\cnc_1_0_0.hex
del /q .\output\cnc_1_0_0.map
del /q .\output\*.d
del /q .\output\*.o
.\make\make.exe -f Makefile_32MZ2048ECG144 clean
.\make\make.exe -f Makefile_32MZ2048ECG144 all -j8
copy .\output\cnc_1_0_0.hex .\output\cnc_1_0_0_32MZ2048ECG144.hex
copy .\output\cnc_1_0_0.map .\output\cnc_1_0_0_32MZ2048ECG144.map
.\make\make.exe -f Makefile_32MZ2048ECG144 clean

del /q .\output\cnc_1_0_0.elf
del /q .\output\cnc_1_0_0.hex
del /q .\output\cnc_1_0_0.map

del /q .\output\*.d
del /q .\output\*.o
del /q .\output\*.elf
del /q .\output\pic_cnc_stepper_1_0_0.hex
del /q .\output\pic_cnc_stepper_1_0_0.map
rem pause