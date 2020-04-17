@echo off
cls
del /q .\out\*.d
del /q .\out\*.o
del /q .\out\*.map
del /q .\out\*.hex
del /q .\out\*.elf

.\make\make.exe -f Makefile_440F256H clean
.\make\make.exe -f Makefile_440F256H all -j8
copy .\out\EthernetBootloader_updater.hex .\out\EthernetBootloader_updater_440F256H.hex
copy .\out\EthernetBootloader_updater.map .\out\EthernetBootloader_updater_440F256H.map
.\make\make.exe -f Makefile_440F256H clean

del /q .\out\EthernetBootloader_470F512H.hex
del /q .\out\EthernetBootloader_470F512H.map
.\make\make.exe -f Makefile_470F512H clean
.\make\make.exe -f Makefile_470F512H all -j8
copy .\out\EthernetBootloader_updater.hex .\out\EthernetBootloader_updater_470F512H.hex
copy .\out\EthernetBootloader_updater.map .\out\EthernetBootloader_updater_470F512H.map
.\make\make.exe -f Makefile_470F512H clean

del /q .\out\EthernetBootloader_updater.map
del /q .\out\*.d
