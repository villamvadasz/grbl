@echo off
cls

del /q .\out\*.d
del /q .\out\*.o
del /q .\out\*.map
del /q .\out\*.hex
del /q .\out\*.elf

del /q .\out\USBHIDBootloader_440F256H.hex
del /q .\out\USBHIDBootloader_440F256H.map
.\make\make.exe -f Makefile_440F256H clean
.\make\make.exe -f Makefile_440F256H all -j8
copy .\out\USBHIDBootloader.hex .\out\USBHIDBootloader_440F256H.hex
copy .\out\USBHIDBootloader.map .\out\USBHIDBootloader_440F256H.map
.\make\make.exe -f Makefile_440F256H clean

del /q .\out\USBHIDBootloader_470F512H.hex
del /q .\out\USBHIDBootloader_470F512H.map
.\make\make.exe -f Makefile_470F512H clean
.\make\make.exe -f Makefile_470F512H all -j8
copy .\out\USBHIDBootloader.hex .\out\USBHIDBootloader_470F512H.hex
copy .\out\USBHIDBootloader.map .\out\USBHIDBootloader_470F512H.map
.\make\make.exe -f Makefile_470F512H clean

del /q .\out\USBHIDBootloader_460F512L.hex
del /q .\out\USBHIDBootloader_460F512L.map
.\make\make.exe -f Makefile_460F512L clean
.\make\make.exe -f Makefile_460F512L all -j8
copy .\out\USBHIDBootloader.hex .\out\USBHIDBootloader_460F512L.hex
copy .\out\USBHIDBootloader.map .\out\USBHIDBootloader_460F512L.map
.\make\make.exe -f Makefile_460F512L clean
del /q .\out\USBHIDBootloader.map

del /q .\out\USBHIDBootloader_470F512L.hex
del /q .\out\USBHIDBootloader_470F512L.map
.\make\make.exe -f Makefile_470F512L clean
.\make\make.exe -f Makefile_470F512L all -j8
copy .\out\USBHIDBootloader.hex .\out\USBHIDBootloader_470F512L.hex
copy .\out\USBHIDBootloader.map .\out\USBHIDBootloader_470F512L.map
.\make\make.exe -f Makefile_470F512L clean
del /q .\out\USBHIDBootloader.map
del /q .\out\*.d
