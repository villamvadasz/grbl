@echo off
cls
cd doc
call copy_files.bat
cd ..
del /q .\output\cnc_1_0_0_440F256H.hex
del /q .\output\cnc_1_0_0_440F256H.map
del /q .\output\cnc_1_0_0_460F512L.hex
del /q .\output\cnc_1_0_0_460F512L.map
del /q .\output\cnc_1_0_0_470F512H.hex
del /q .\output\cnc_1_0_0_470F512H.map
del /q .\output\cnc_1_0_0_470F512L.hex
del /q .\output\cnc_1_0_0_470F512L.map
del /q .\output\cnc_1_0_0.hex
.\make\make.exe -f Makefile_440F256H clean
.\make\make.exe -f Makefile_440F256H all -j8
copy .\output\cnc_1_0_0.hex .\output\cnc_1_0_0_440F256H.hex
copy .\output\cnc_1_0_0.map .\output\cnc_1_0_0_440F256H.map
.\make\make.exe -f Makefile_440F256H clean

del /q .\output\cnc_1_0_0.hex
.\make\make.exe -f Makefile_460F512L clean
.\make\make.exe -f Makefile_460F512L all -j8
copy .\output\cnc_1_0_0.hex .\output\cnc_1_0_0_460F512L.hex
copy .\output\cnc_1_0_0.map .\output\cnc_1_0_0_460F512L.map
.\make\make.exe -f Makefile_460F512L clean

del /q .\output\cnc_1_0_0.hex
.\make\make.exe -f Makefile_470F512H clean
.\make\make.exe -f Makefile_470F512H all -j8
copy .\output\cnc_1_0_0.hex .\output\cnc_1_0_0_470F512H.hex
copy .\output\cnc_1_0_0.map .\output\cnc_1_0_0_470F512H.map
.\make\make.exe -f Makefile_470F512H clean

del /q .\output\cnc_1_0_0.hex
.\make\make.exe -f Makefile_470F512L clean
.\make\make.exe -f Makefile_470F512L all -j8
copy .\output\cnc_1_0_0.hex .\output\cnc_1_0_0_470F512L.hex
copy .\output\cnc_1_0_0.map .\output\cnc_1_0_0_470F512L.map
.\make\make.exe -f Makefile_470F512L clean
del /q .\output\cnc_1_0_0.elf
del /q .\output\cnc_1_0_0.map
del /q .\output\cnc_1_0_0.hex

del /q .\output\*.d
pause
