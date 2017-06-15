@echo off
cls
cd doc
call copy_files.bat
cd ..
del /q .\output\cnc_1_0_0_440F256H.hex
del /q .\output\cnc_1_0_0_440F256H.map
del /q .\output\cnc_1_0_0_440F256H_arduino.hex
del /q .\output\cnc_1_0_0_440F256H_arduino.map
del /q .\output\cnc_1_0_0_440F256H_old_grbl.hex
del /q .\output\cnc_1_0_0_440F256H_old_grbl.map
del /q .\output\cnc_1_0_0_440F256H_simulator.hex
del /q .\output\cnc_1_0_0_440F256H_simulator.map
del /q .\output\cnc_1_0_0_440F256H_debug.hex
del /q .\output\cnc_1_0_0_440F256H_debug.map
del /q .\output\cnc_1_0_0_460F512L.hex
del /q .\output\cnc_1_0_0_460F512L.map
del /q .\output\cnc_1_0_0_470F512H.hex
del /q .\output\cnc_1_0_0_470F512H.map
del /q .\output\cnc_1_0_0_470F512L.hex
del /q .\output\cnc_1_0_0_470F512L.map
del /q .\output\cnc_1_0_0.hex
del /q .\output\cnc_1_0_0.map
del /q .\output\*.d
del /q .\output\*.o
.\make\make.exe -f Makefile_440F256H_debug clean
.\make\make.exe -f Makefile_440F256H_debug all -j8
copy .\output\cnc_1_0_0.hex .\output\cnc_1_0_0_440F256H_debug.hex
copy .\output\cnc_1_0_0.map .\output\cnc_1_0_0_440F256H_debug.map
.\make\make.exe -f Makefile_440F256H_debug clean
del /q .\output\cnc_1_0_0.elf
del /q .\output\cnc_1_0_0.hex
del /q .\output\cnc_1_0_0.map

del /q .\output\*.d
del /q .\output\*.o
del /q .\output\*.elf
del /q .\output\pic_cnc_stepper_1_0_0.hex
del /q .\output\pic_cnc_stepper_1_0_0.map
rem pause