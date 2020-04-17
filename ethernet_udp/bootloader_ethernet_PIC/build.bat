@echo off
cls
del /q .\out\*.d
del /q .\out\*.o
del /q .\out\*.map
del /q .\out\*.hex
del /q .\out\*.elf

.\make\make.exe -f Makefile_440F256H clean
.\make\make.exe -f Makefile_440F256H all -j8
copy .\out\EthernetBootloader.hex .\out\EthernetBootloader_440F256H.hex
copy .\out\EthernetBootloader.map .\out\EthernetBootloader_440F256H.map
.\make\make.exe -f Makefile_440F256H clean

del /q .\out\EthernetBootloader_470F512H.hex
del /q .\out\EthernetBootloader_470F512H.map
.\make\make.exe -f Makefile_470F512H clean
.\make\make.exe -f Makefile_470F512H all -j8
copy .\out\EthernetBootloader.hex .\out\EthernetBootloader_470F512H.hex
copy .\out\EthernetBootloader.map .\out\EthernetBootloader_470F512H.map
.\make\make.exe -f Makefile_470F512H clean

del /q .\out\EthernetBootloader_460F512L.hex
del /q .\out\EthernetBootloader_460F512L.map
.\make\make.exe -f Makefile_460F512L clean
.\make\make.exe -f Makefile_460F512L all -j8
copy .\out\EthernetBootloader.hex .\out\EthernetBootloader_460F512L.hex
copy .\out\EthernetBootloader.map .\out\EthernetBootloader_460F512L.map
.\make\make.exe -f Makefile_460F512L clean
del /q .\out\EthernetBootloader.map

del /q .\out\EthernetBootloader_470F512L.hex
del /q .\out\EthernetBootloader_470F512L.map
.\make\make.exe -f Makefile_470F512L clean
.\make\make.exe -f Makefile_470F512L all -j8
copy .\out\EthernetBootloader.hex .\out\EthernetBootloader_470F512L.hex
copy .\out\EthernetBootloader.map .\out\EthernetBootloader_470F512L.map
.\make\make.exe -f Makefile_470F512L clean

del /q .\out\EthernetBootloader.map
del /q .\out\*.d
