@echo off
cls
echo ***************************************
echo Flashing with MPLAB manualy a working BL + APP
echo ***************************************
echo.
echo There is a small difference in the read out file by Mplab
echo Import the bootloader and export it.
echo Import the bootloader, flash it, read it, and export it
echo There is a small diff, if this is corrected in the readout, then the hex file will work
echo.
echo ***************************************
echo Flashing with bootloader over USB
echo ***************************************
echo.
echo Build project with build_all.bat
echo Flash bootloader to PIC with ICD3.
echo Use bootloader to flash serial number and aes key
echo Flash App with bootloader using the signature
echo.
echo ***************************************
echo Flashing over FOTA
echo ***************************************
echo.
echo Upload file to http://kyrk.villamvadasz.hu/_pic_sw_bootloader
echo UIDXXXXX.nxt files shows the UIDXXXXX which file is the next to flash
echo YYYYMMDDHHMM_UIDXXXXX_SIGNATUREnnnnnnnnnnnnnnnnnnnnnnn.hex
echo.

set datetimef=%date:~-4%%date:~3,2%%date:~0,2%%time:~0,2%%time:~3,2%
set YYYYMMDDHHMM=%datetimef%
set SIGNEDFILEDEXTENSION=hsx

echo Signatures>.\output\signature_bootloader.txt
echo Signatures>.\output\signature_fota.txt
del /q .\output\*.nxt
del /q .\output\*.enc
del /q .\output\*.dec
del /q .\output\*.%SIGNEDFILEDEXTENSION%


set FILE_NAME=cnc_1_0_0_440F256H.hex
set AES_KEY_SIGNATURE=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set AES_KEY_ENCRYPT=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set SERIAL_NUMBER=FFFFFFFF
set BOOTLOADER_UID=NA
call :signfile_bootloader

set FILE_NAME=cnc_1_0_0_440F256H_simulator.hex
set AES_KEY_SIGNATURE=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set AES_KEY_ENCRYPT=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set SERIAL_NUMBER=FFFFFFFF
set BOOTLOADER_UID=NA
call :signfile_bootloader

set FILE_NAME=cnc_1_0_0_440F256H_simulator_1_0_2.hex
set AES_KEY_SIGNATURE=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set AES_KEY_ENCRYPT=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set SERIAL_NUMBER=FFFFFFFF
set BOOTLOADER_UID=NA
call :signfile_bootloader

set FILE_NAME=cnc_1_0_0_460F512L.hex
set AES_KEY_SIGNATURE=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set AES_KEY_ENCRYPT=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set SERIAL_NUMBER=FFFFFFFF
set BOOTLOADER_UID=NA
call :signfile_bootloader

set FILE_NAME=cnc_1_0_0_470F512H.hex
set AES_KEY_SIGNATURE=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set AES_KEY_ENCRYPT=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set SERIAL_NUMBER=FFFFFFFF
set BOOTLOADER_UID=NA
call :signfile_bootloader

set FILE_NAME=cnc_1_0_0_470F512L.hex
set AES_KEY_SIGNATURE=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set AES_KEY_ENCRYPT=FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
set SERIAL_NUMBER=FFFFFFFF
set BOOTLOADER_UID=NA
call :signfile_bootloader

set FILE_NAME=cnc_1_0_0_440F256H.hex
set AES_KEY_SIGNATURE=9e4810e6f37a2a42c1a54a77e10c48e1
set AES_KEY_ENCRYPT=da3262294f2da2bdb346182282104df2
set AES_KEY_ENCRYPT=%AES_KEY_SIGNATURE%
set SERIAL_NUMBER=DE000002
set BOOTLOADER_UID=NA
call :signfile_bootloader

set FILE_NAME=cnc_1_0_0_795F512H.hex
set AES_KEY_SIGNATURE=9e4810e6f37a2a42c1a54a77e10c48e1
set AES_KEY_ENCRYPT=da3262294f2da2bdb346182282104df2
set AES_KEY_ENCRYPT=%AES_KEY_SIGNATURE%
set SERIAL_NUMBER=DE000002
set BOOTLOADER_UID=NA
call :sign_enc_file_bootloader

set FILE_NAME=cnc_1_0_0_795F512H.hex
set AES_KEY_SIGNATURE=05cf0d46621dc94caac88a97579f2c3f
set AES_KEY_ENCRYPT=15d1b10d061031e25100ffd2ca04e8e8
set AES_KEY_ENCRYPT=%AES_KEY_SIGNATURE%
set SERIAL_NUMBER=DE000003
set BOOTLOADER_UID=NA
call :sign_enc_file_bootloader

set FILE_NAME=cnc_1_0_0_795F512H_simulator.hex
set AES_KEY_SIGNATURE=05cf0d46621dc94caac88a97579f2c3f
set AES_KEY_ENCRYPT=15d1b10d061031e25100ffd2ca04e8e8
set AES_KEY_ENCRYPT=%AES_KEY_SIGNATURE%
set SERIAL_NUMBER=DE000003
set BOOTLOADER_UID=NA
call :sign_enc_file_bootloader

set FILE_NAME=cnc_1_0_0_795F512H_sdcard.hex
set AES_KEY_SIGNATURE=3178d5f737dfd407f120db7518a6a657
set AES_KEY_ENCRYPT=53c12b7750c1b18f0cfe0d73ca534830
set AES_KEY_ENCRYPT=%AES_KEY_SIGNATURE%
set SERIAL_NUMBER=DE000004
set BOOTLOADER_UID=4D2E6F08
call :sign_enc_file_bootloader

set FILE_NAME=cnc_1_0_0_795F512H_sdcard.hex
set AES_KEY_SIGNATURE=3178d5f737dfd407f120db7518a6a657
set AES_KEY_ENCRYPT=53c12b7750c1b18f0cfe0d73ca534830
set SERIAL_NUMBER=DE000004
set BOOTLOADER_UID=4D2E6F08
call :signfile_fota

set FILE_NAME=cnc_1_0_0_795F512H_sdcard.hex
set AES_KEY_SIGNATURE=1c0adf938d888ed04213ae0af8dd01a4
set AES_KEY_ENCRYPT=c73144a97f0bbeb81b4e6f9ca0837bd8
set SERIAL_NUMBER=DE000005
set BOOTLOADER_UID=4D2E6F08
call :signfile_fota


del /q .\output\temp.tmp
EXIT /B 0


:signfile_bootloader
echo|set /p="%FILE_NAME% ">>.\output\signature_bootloader.txt
echo|set /p="%SERIAL_NUMBER% ">>.\output\signature_bootloader.txt
.\signer_pc\bootloader_signer_4.exe -b .\output\%FILE_NAME% %AES_KEY_SIGNATURE% %SERIAL_NUMBER% >>.\output\signature_bootloader.txt
copy /Y .\output\%FILE_NAME% .\output\%FILE_NAME%_%SERIAL_NUMBER%.hex
del /q .\output\%FILE_NAME%_%SERIAL_NUMBER%.hex
echo.>>.\output\signature_bootloader.txt
EXIT /B 0

:sign_enc_file_bootloader
echo|set /p="%FILE_NAME% ">>.\output\signature_bootloader.txt
echo|set /p="%SERIAL_NUMBER% ">>.\output\signature_bootloader.txt
.\signer_pc\bootloader_signer_4.exe -b .\output\%FILE_NAME% %AES_KEY_SIGNATURE% %SERIAL_NUMBER% >>.\output\signature_bootloader.txt
copy /Y .\output\%FILE_NAME% .\output\%FILE_NAME%_%SERIAL_NUMBER%.hex
.\signer_pc\bootloader_signer_4.exe -e .\output\%FILE_NAME%_%SERIAL_NUMBER%.hex %AES_KEY_ENCRYPT% %SERIAL_NUMBER%
del /q .\output\%FILE_NAME%_%SERIAL_NUMBER%.hex
echo.>>.\output\signature_bootloader.txt
EXIT /B 0


:signfile_fota
echo|set /p="%FILE_NAME% ">>.\output\signature_fota.txt
echo|set /p="%SERIAL_NUMBER% ">>.\output\signature_fota.txt
IF "%BOOTLOADER_UID%"=="NA" GOTO SKIP_THIS_PART
.\signer_pc\bootloader_signer_4.exe -f .\output\%FILE_NAME% %AES_KEY_SIGNATURE% %SERIAL_NUMBER% >.\output\temp.tmp
for /f "delims=" %%x in (.\output\temp.tmp) do set signaturetmp=%%x
set file=.\output\%FILE_NAME%
for %%A in ("%file%") do set "size=%%~zA"
set SIGNEDFILENAME=%YYYYMMDDHHMM%_%BOOTLOADER_UID%_%SERIAL_NUMBER%_%size%
if not exist .\output\%SIGNEDFILENAME%.%SIGNEDFILEDEXTENSION% (
	copy /Y .\output\%FILE_NAME% .\output\%SIGNEDFILENAME%.%SIGNEDFILEDEXTENSION%>.\output\temp.tmp
)
.\signer_pc\bootloader_signer_4.exe -e .\output\%SIGNEDFILENAME%.%SIGNEDFILEDEXTENSION% %AES_KEY_ENCRYPT% %SERIAL_NUMBER%
echo %SIGNEDFILENAME%.%SIGNEDFILEDEXTENSION% %BOOTLOADER_UID% %SERIAL_NUMBER% %signaturetmp%>>.\output\%BOOTLOADER_UID%.nxt
:SKIP_THIS_PART

.\signer_pc\bootloader_signer_4.exe -f .\output\%FILE_NAME% %AES_KEY_SIGNATURE% %SERIAL_NUMBER% >>.\output\signature_fota.txt
echo.>>.\output\signature_fota.txt
EXIT /B 0
