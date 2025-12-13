#include <stdio.h>
#include <string.h>
#include "c_fatfs.h"
#include "SDCard.h"

#include "mal.h"
#include "diskio.h"

BYTE response_R(BYTE);
BYTE send_cmd(BYTE *);

static unsigned short /*__attribute__ ((progmem))*/
crc_tab[256]={
0,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,0x1231,
0x210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,0x2462,
0x3443,0x420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,0x3653,0x2672,
0x1611,0x630,0x76d7,0x66f6,0x5695,0x46b4,0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,0x48c4,0x58e5,0x6886,
0x78a7,0x840,0x1861,0x2802,0x3823,0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,0x5af5,0x4ad4,0x7ab7,0x6a96,
0x1a71,0xa50,0x3a33,0x2a12,0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,
0x3c03,0xc60,0x1c41,0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,
0x1e51,0xe70,0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,
0xe16f,0x1080,0xa1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,0x34e2,
0x24c3,0x14a0,0x481,0x7466,0x6447,0x5424,0x4405,0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,0x26d3,0x36f2,
0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,0x5844,0x4865,0x7806,
0x6827,0x18c0,0x8e1,0x3882,0x28a3,0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,0x4a75,0x5a54,0x6a37,0x7a16,
0x0af1,0x1ad0,0x2ab3,0x3a92,0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,
0x2c83,0x1ce0,0xcc1,0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0xed1,0x1ef0
};


//-------------------------------------------------------------------------
BYTE response_buffer[20];
BYTE RCA[2];
BYTE cmd_buffer[5];

BYTE cmd0[5] = {0x40,0x00,0x00,0x00,0x00};//Reset SD Card
BYTE cmd55[5] = {0x77,0x00,0x00,0x00,0x00};//Next CMD is ASC
BYTE cmd2[5] = {0x42,0x00,0x00,0x00,0x00};//asks to send the CID numbers
BYTE cmd3[5] = {0x43,0x00,0x00,0x00,0x00};//Send RCA
BYTE cmd7[5] = {0x47,0x00,0x00,0x00,0x00};//Select one card,put it into Transfer State
BYTE cmd9[5] = {0x49,0x00,0x00,0x00,0x00};//Ask send CSD
BYTE cmd16[5] = {0x50,0x00,0x00,0x02,0x00};//Select a block length
BYTE cmd17[5] = {0x51,0x00,0x00,0x00,0x00};//Read a single block
BYTE acmd6[5] = {0x46,0x00,0x00,0x00,0x02};//SET BUS WIDTH
BYTE cmd24[5] = {0x58,0x00,0x00,0x00,0x00};//Write a single block

BYTE acmd41[5] = {0x69,0x0f,0xf0,0x00,0x00};//Active Card’s ini process
BYTE acmd42[5] = {0x6A,0x0f,0xf0,0x00,0x00};//Disable pull up on Dat3
BYTE acmd51[5] = {0x73,0x00,0x00,0x00,0x00};//Read SCR(Configuration Reg)
//-------------------------------------------------------------------------

// SD Card Set I/O Direction
#define SD_CLK_IN 		SD4BIT_CLK_TRIS = 1
#define SD_CLK_OUT 		SD4BIT_CLK_TRIS = 0
#define SD_CMD_IN 		SD4BIT_CMD_TRIS = 1
#define SD_CMD_OUT 		SD4BIT_CMD_TRIS = 0
#define SD_DAT0_IN 		SD4BIT_DAT0_TRIS = 1
#define SD_DAT0_OUT 	SD4BIT_DAT0_TRIS = 0
#define SD_DAT1_IN 		SD4BIT_DAT1_TRIS = 1
#define SD_DAT1_OUT 	SD4BIT_DAT1_TRIS = 0
#define SD_DAT2_IN 		SD4BIT_DAT2_TRIS = 1
#define SD_DAT2_OUT 	SD4BIT_DAT2_TRIS = 0
#define SD_DAT3_IN 		SD4BIT_DAT3_TRIS = 1
#define SD_DAT3_OUT 	SD4BIT_DAT3_TRIS = 0
// SD Card Output High/Low
#define SD_CMD_LOW 		SD4BIT_CMD_LAT = 0
#define SD_CMD_HIGH 	SD4BIT_CMD_LAT = 1
#define SD_DAT0_LOW 	SD4BIT_DAT0_LAT = 0
#define SD_DAT0_HIGH 	SD4BIT_DAT0_LAT = 1
#define SD_DAT1_LOW 	SD4BIT_DAT1_LAT = 0
#define SD_DAT1_HIGH 	SD4BIT_DAT1_LAT = 1
#define SD_DAT2_LOW 	SD4BIT_DAT2_LAT = 0
#define SD_DAT2_HIGH 	SD4BIT_DAT2_LAT = 1
#define SD_DAT3_LOW 	SD4BIT_DAT3_LAT = 0
#define SD_DAT3_HIGH 	SD4BIT_DAT3_LAT = 1

#define SD_CLK_LOW 		SD4BIT_CLK_LAT = 0
#define SD_CLK_HIGH 	SD4BIT_CLK_LAT = 1
// SD Card Input Read
#define SD_TEST_CMD 	SD4BIT_CMD_PORT
#define SD_TEST_DAT0 	SD4BIT_DAT0_PORT
#define SD_TEST_DAT1 	SD4BIT_DAT1_PORT
#define SD_TEST_DAT2 	SD4BIT_DAT2_PORT
#define SD_TEST_DAT3 	SD4BIT_DAT3_PORT

DWORD MDD_SD4BIT_finalLBA = 0;
WORD gMediaSectorSize_SD4BIT = 0;
BYTE gSDMode_SD4BIT = 0;
MEDIA_INFORMATION mediaInformation;
ASYNC_IO ioInfo; //Declared global context, for fast/code efficient access
BYTE MDD_SD4BIT_MediaDetected = 0;
uint8 do_sd_4bit_1ms = 0;
uint8 do_sd_4bit_100ms = 0;

uint8 do_sd_4bit_testOutput = 0;

static void do_MDD_SD4BIT_MediaDetect(void);
static void SdCardInit(void);

void init_sd_4bit(void) {
	SD_CLK_OUT;_sync();
	SD_CMD_IN;_sync(); SD_CMD_LOW;_sync();
	SD_DAT0_IN;_sync(); SD_DAT0_LOW;_sync();
	SD_DAT1_IN;_sync(); SD_DAT1_LOW;_sync();
	SD_DAT2_IN;_sync(); SD_DAT2_LOW;_sync();
	SD_DAT3_IN;_sync(); SD_DAT3_LOW;_sync();
}

void do_sd_4bit(void) {
	if (do_sd_4bit_1ms) {
		do_sd_4bit_1ms = 0;
	}
	if (do_sd_4bit_100ms) {
		do_sd_4bit_100ms = 0;
		do_MDD_SD4BIT_MediaDetect();
	}
}

void isr_sd_4bit_1ms(void) {
	static uint8 sd_4bit_100msCnt = 0;
	sd_4bit_100msCnt++;
	if (sd_4bit_100msCnt >= 100) {
		sd_4bit_100msCnt = 0;
		do_sd_4bit_100ms = 1;
	}

	do_sd_4bit_1ms = 1;
}

inline void Ncr(void) {
	SD_CMD_IN;_sync();
	SD_CLK_LOW;_sync();
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync();
	SD_CLK_HIGH;_sync();
}
//-------------------------------------------------------------------------
inline void Ncc(void)
{
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
	SD_CLK_LOW;_sync(); 
	SD_CLK_HIGH;_sync();
}
//-------------------------------------------------------------------------

static BYTE SdCardInit(void)
{
	DWORD c_size;
	BYTE c_size_mult;
	BYTE block_len;

	BYTE x,y;
	SD_CLK_OUT;_sync();
	SD_CMD_IN;_sync(); SD_CMD_LOW;_sync();
	SD_DAT0_IN;_sync(); SD_DAT0_LOW;_sync();
	SD_DAT1_IN;_sync(); SD_DAT1_LOW;_sync();
	SD_DAT2_IN;_sync(); SD_DAT2_LOW;_sync();
	SD_DAT3_IN;_sync(); SD_DAT3_LOW;_sync();

	SD_CMD_OUT;_sync();
	SD_CLK_HIGH;_sync(); SD_CMD_HIGH;_sync();
	
	for (x = 0; x < 40; x++) {
		Ncr();
	}
	memcpy(cmd_buffer, cmd0, 5);
	y = send_cmd(cmd_buffer);//Reset SD Card
	do {
		for (x = 0;x < 40; x++) asm volatile ("nop");//delay
		Ncc();
		memcpy(cmd_buffer, cmd55, 5);
		y = send_cmd(cmd_buffer);//Next CMD is ASC
		Ncr();
		if (response_R(1) > 1) {
			return 1;//response too long or crc error
		}
		Ncc();
		memcpy(cmd_buffer, acmd41, 5);
		y = send_cmd(cmd_buffer);//Active Card’s ini process
		Ncr();
	} while (response_R(3) == 1);

	Ncc();
	memcpy(cmd_buffer, cmd2, 5);
	y = send_cmd(cmd_buffer);
	Ncr();
	if (response_R(2) > 1) {
		return 2;
	}

	Ncc();
	memcpy(cmd_buffer, cmd3, 5);
	y = send_cmd(cmd_buffer);//Send RCA
	Ncr();
	if (response_R(6) > 1) {
		return 3;
	}

	RCA[0] = response_buffer[1];
	RCA[1] = response_buffer[2];
	Ncc();
	memcpy(cmd_buffer, cmd9, 5);
	cmd_buffer[1] = RCA[0];
	cmd_buffer[2] = RCA[1];
	y = send_cmd(cmd_buffer);//Ask send CSD
	Ncr();
	if (response_R(2) > 1) {
		return 4;
	}
	
	if(response_buffer[0] & 0xC0)	//Check CSD_STRUCTURE field for v2+ struct device
	{
		//Must be a v2 device (or a reserved higher version, that doesn't currently exist)

		//Extract the C_SIZE field from the response.  It is a 22-bit number in bit position 69:48.  This is different from v1.  
		//It spans bytes 7, 8, and 9 of the response.
		c_size = (((DWORD)response_buffer[7] & 0x3F) << 16) | ((WORD)response_buffer[8] << 8) | response_buffer[9];
		
		MDD_SD4BIT_finalLBA = ((DWORD)(c_size + 1) * (WORD)(1024u)) - 1; //-1 on end is correction factor, since LBA = 0 is valid.
	}
	else //if(CSDResponse[0] & 0xC0)	//Check CSD_STRUCTURE field for v1 struct device
	{
		//Must be a v1 device.
		//Extract the C_SIZE field from the response.  It is a 12-bit number in bit position 73:62.  
		//Although it is only a 12-bit number, it spans bytes 6, 7, and 8, since it isn't byte aligned.
		c_size = ((DWORD)response_buffer[6] << 16) | ((WORD)response_buffer[7] << 8) | response_buffer[8];	//Get the bytes in the correct positions
		c_size &= 0x0003FFC0;	//Clear all bits that aren't part of the C_SIZE
		c_size = c_size >> 6;	//Shift value down, so the 12-bit C_SIZE is properly right justified in the DWORD.
		
		//Extract the C_SIZE_MULT field from the response.  It is a 3-bit number in bit position 49:47.
		c_size_mult = ((WORD)((response_buffer[9] & 0x03) << 1)) | ((WORD)((response_buffer[10] & 0x80) >> 7));

		//Extract the BLOCK_LEN field from the response. It is a 4-bit number in bit position 83:80.
		block_len = response_buffer[5] & 0x0F;

		block_len = 1 << (block_len - 9); //-9 because we report the size in sectors of 512 bytes each
		
		//Calculate the MDD_SD4BIT_finalLBA (see SD card physical layer simplified spec 2.0, section 5.3.2).
		//In USB mass storage applications, we will need this information to 
		//correctly respond to SCSI get capacity requests (which will cause MDD_SDSPI_ReadCapacity() to get called).
		MDD_SD4BIT_finalLBA = ((DWORD)(c_size + 1) * (WORD)((WORD)1 << (c_size_mult + 2)) * block_len) - 1;	//-1 on end is correction factor, since LBA = 0 is valid.		
	}	
	
	
	Ncc();
	memcpy(cmd_buffer, cmd7, 5);
	cmd_buffer[1] = RCA[0];
	cmd_buffer[2] = RCA[1];
	y = send_cmd(cmd_buffer);//Select one card, put it into Transfer State
	Ncr();
	if (response_R(1) > 1) {
		return 5;
	}
	
	//change to 4bit bus width
	do {
		for (x = 0; x < 40; x++) asm volatile ("nop");//delay
		Ncc();
		memcpy(cmd_buffer, cmd55, 5);
		cmd_buffer[1] = RCA[0];
		cmd_buffer[2] = RCA[1];
		y = send_cmd(cmd_buffer);//Next CMD is ASC
		Ncr();
		if (response_R(1) > 1) {
			return 6;//response too long or crc error
		}

		Ncc();
		memcpy(cmd_buffer, acmd6, 5);
		y = send_cmd(cmd_buffer);  
		Ncr();
	} while (response_R(1) == 1);
	
	Ncc();
	memcpy(cmd_buffer, cmd16, 5);
	y = send_cmd(cmd_buffer);//Select a block length 512Byte
	Ncr();
	if (response_R(1) > 1) {
		return 7;
	}

	return 0;
}
//-------------------------------------------------------------------------
BYTE SdReadBlock(UINT32 Block, BYTE *buff)
{	  
	{ //ReadBlock-Cmd
		Ncc();
		cmd_buffer[0] = cmd17[0];
		cmd_buffer[1] = (Block >> 15)&0xff;
		cmd_buffer[2] = (Block >> 7)&0xff;
		cmd_buffer[3] = (Block << 1)&0xff;
		cmd_buffer[4] = 0;
		send_cmd(cmd_buffer);
		Ncr();
	}
	unsigned int timeout = 0;
	while(1) { //Wait until startbit comes
		SD_CLK_LOW;_sync();
		SD_CLK_HIGH;_sync();
		if(!(SD_TEST_DAT0)) 
			if(!(SD_TEST_DAT1))
				if(!(SD_TEST_DAT2))
					if(!(SD_TEST_DAT3)) break;
		if (timeout++ > 65534) return 1;
	}
 	int i; 
	for (i = 0; i < 512; i++) {// Read 512 Bytes
		register unsigned char c = 0;
		SD_CLK_LOW;_sync(); SD_CLK_HIGH;_sync();
		if(SD_TEST_DAT0) c |= 0x10;
		if(SD_TEST_DAT1) c |= 0x20;
		if(SD_TEST_DAT2) c |= 0x40;
		if(SD_TEST_DAT3) c |= 0x80;

		SD_CLK_LOW;_sync(); SD_CLK_HIGH;_sync();
		if(SD_TEST_DAT0) c |= 0x01;
		if(SD_TEST_DAT1) c |= 0x02;
		if(SD_TEST_DAT2) c |= 0x04;
		if(SD_TEST_DAT3) c |= 0x08; 
		*buff++=c;
	}
	for (i = 0; i < 4; i++) { //Dummy read CRC
		SD_CLK_LOW;_sync(); SD_CLK_HIGH;_sync();	
		SD_CLK_LOW;_sync(); SD_CLK_HIGH;_sync();
		SD_CLK_LOW;_sync(); SD_CLK_HIGH;_sync();
		SD_CLK_LOW;_sync(); SD_CLK_HIGH;_sync();
	}
	return 0;
}
//-------------------------------------------------------------------------

BYTE SdWriteBlock(UINT32 Block, BYTE *buff)
{
	UINT32 i,j;
	unsigned short crc[4] = {0};
	{ // WriteBlock-Cmd
		Ncc();
		cmd_buffer[0] = cmd24[0];
		cmd_buffer[1] = (Block >> 15) & 0xff;
		cmd_buffer[2] = (Block >> 7) & 0xff;
		cmd_buffer[3] = (Block << 1) & 0xff;
		cmd_buffer[4] = 0;
		send_cmd(cmd_buffer);
	}
	for (i = 0; i < 100; i++) { 
		SD_CLK_LOW;_sync(); 
		SD_CLK_HIGH;_sync(); 
	} 
	SD_CLK_LOW;_sync(); //send startbit
	SD_DAT0_OUT;_sync(); SD_DAT0_LOW;_sync();
	SD_DAT1_OUT;_sync(); SD_DAT1_LOW;_sync();
	SD_DAT2_OUT;_sync(); SD_DAT2_LOW;_sync();
	SD_DAT3_OUT;_sync(); SD_DAT3_LOW;_sync();
	SD_CLK_HIGH;_sync();
	unsigned char c;
	char* buff_adr  = buff;
	for(i = 0; i < 128; i++) {// Computer 4 CRCs
		register unsigned char a = 0, b = 0, c = 0, d = 0;
		if (*buff & 0x10) a |= 0x80; if (*buff & 0x20) b |= 0x80;
		if (*buff & 0x40) c |= 0x80; if (*buff & 0x80) d |= 0x80;
		if (*buff & 0x01) a |= 0x40; if (*buff & 0x02) b |= 0x40;
		if (*buff & 0x04) c |= 0x40; if (*buff & 0x08) d |= 0x40;
		buff++;						
		if (*buff & 0x10) a |= 0x20; if (*buff & 0x20) b |= 0x20;
		if (*buff & 0x40) c |= 0x20; if (*buff & 0x80) d |= 0x20;			
		if (*buff & 0x01) a |= 0x10; if (*buff & 0x02) b |= 0x10;
		if (*buff & 0x04) c |= 0x10; if (*buff & 0x08) d |= 0x10;
		buff++;
		if (*buff & 0x10) a |= 0x08; if (*buff & 0x20) b |= 0x08;
		if (*buff & 0x40) c |= 0x08; if (*buff & 0x80) d |= 0x08;
		if (*buff & 0x01) a |= 0x04; if (*buff & 0x02) b |= 0x04;
		if (*buff & 0x04) c |= 0x04; if (*buff & 0x08) d |= 0x04;			
		buff++;						
		if (*buff & 0x10) a |= 0x02; if (*buff & 0x20) b |= 0x02;
		if (*buff & 0x40) c |= 0x02; if (*buff & 0x80) d |= 0x02;
		if (*buff & 0x01) a |= 0x01; if (*buff & 0x02) b |= 0x01;
		if (*buff & 0x04) c |= 0x01; if (*buff & 0x08) d |= 0x01;
		buff++;

		crc[0]=crc_tab[(crc[0]>>8)^a]^(crc[0]<<8);
		crc[1]=crc_tab[(crc[1]>>8)^b]^(crc[1]<<8);
		crc[2]=crc_tab[(crc[2]>>8)^c]^(crc[2]<<8);
		crc[3]=crc_tab[(crc[3]>>8)^d]^(crc[3]<<8);
	}
	buff = buff_adr;
	for (i = 0; i < 512; i++) {//transfer 512 bytes
		c = *buff;
		SD_CLK_LOW;_sync();
		if (c & 0x10) {SD_DAT0_HIGH;_sync();} else {SD_DAT0_LOW;_sync();}
		if (c & 0x20) {SD_DAT1_HIGH;_sync();} else {SD_DAT1_LOW;_sync();}
		if (c & 0x40) {SD_DAT2_HIGH;_sync();} else {SD_DAT2_LOW;_sync();}
		if (c & 0x80) {SD_DAT3_HIGH;_sync();} else {SD_DAT3_LOW;_sync();}
		SD_CLK_HIGH;_sync();
		asm volatile ("nop");
		SD_CLK_LOW; _sync();	   
		if (c & 0x01) {SD_DAT0_HIGH;_sync();} else {SD_DAT0_LOW;_sync();}
		if (c & 0x02) {SD_DAT1_HIGH;_sync();} else {SD_DAT1_LOW;_sync();}
		if (c & 0x04) {SD_DAT2_HIGH;_sync();} else {SD_DAT2_LOW;_sync();}
		if (c & 0x08) {SD_DAT3_HIGH;_sync();} else {SD_DAT3_LOW;_sync();}
		SD_CLK_HIGH;
		buff++;
	}
 	for (j = 0; j < 16; j++) {//write CRC
		SD_CLK_LOW;_sync();
		if (crc[0] & 0x8000) {SD_DAT0_HIGH;_sync();} else {SD_DAT0_LOW;_sync();}
		if (crc[1] & 0x8000) {SD_DAT1_HIGH;_sync();} else {SD_DAT1_LOW;_sync();}
		if (crc[2] & 0x8000) {SD_DAT2_HIGH;_sync();} else {SD_DAT2_LOW;_sync();}
		if (crc[3] & 0x8000) {SD_DAT3_HIGH;_sync();} else {SD_DAT3_LOW;_sync();}
		SD_CLK_HIGH;_sync();
		crc[0]<<=1; crc[1]<<=1; crc[2]<<=1; crc[3]<<=1;
	}
	c=0;
	SD_CLK_LOW;_sync();
	SD_DAT0_HIGH;_sync(); SD_DAT1_HIGH;_sync(); SD_DAT2_HIGH;_sync(); SD_DAT3_HIGH; _sync();
	SD_CLK_HIGH;_sync();
	SD_DAT0_IN;_sync(); SD_DAT1_IN;_sync(); SD_DAT2_IN;_sync(); SD_DAT3_IN;_sync();
	for(i=0; i<7; i++) { //wait for response
		SD_CLK_LOW;_sync();
		SD_CLK_HIGH;_sync();
		c=c<<1;
		if(SD_TEST_DAT0) c|=0x01;
	}
	Ncc();
	unsigned int timeout = 0;
	while(1) { //wait until card is not busy
		SD_CLK_LOW;_sync();
		SD_CLK_HIGH;_sync();
		if(SD_TEST_DAT0)
			if(SD_TEST_DAT1)
				if(SD_TEST_DAT2)
					if(SD_TEST_DAT3) break;
		if (timeout++ > 65534) return 1;
	}
	return c != 101; //101 means "all right"
}

//-------------------------------------------------------------------------
BYTE response_R(BYTE s)
{
	BYTE a = 0, b = 0, c = 0, r = 0, crc = 0;
	BYTE i, j = 6, k;
	while(1) {
		SD_CLK_LOW;_sync();
		SD_CLK_HIGH;_sync();
		if(!(SD_TEST_CMD)) break;
		if(crc++ > 254) return 2;
	}
	crc =0;
	if (s == 2) j = 17;
	for (k = 0; k < j; k++) {
		c = 0;
		if(k > 0) //for crc culcar
		b = response_buffer[k - 1];
		for (i = 0; i < 8; i++) {
			SD_CLK_LOW;_sync();
			if (a > 0) {
				c <<= 1;
			} else {
				i++;
			}
			a++;
			SD_CLK_HIGH;_sync();
			if(SD_TEST_CMD) c |= 0x01;
			if(k > 0) {
				crc <<= 1;
				if ((crc ^ b) & 0x80) {
					crc ^= 0x09;
				}
				b <<= 1;
				crc &= 0x7f;
			}
		}
		if (s==3) {
			if (k==1 && (!(c & 0x80))) {
				r=1;
			}
		}
		response_buffer[k] = c;
	}
	if (s==1 || s==6)
	if (c != ((crc << 1) + 1)) {
		r=2;
	}

	return r;
}
//-------------------------------------------------------------------------

BYTE send_cmd(BYTE *in)
{
	int i,j;
	BYTE b,crc=0;
	SD_CMD_OUT;_sync();
	for (i = 0; i < 5; i++) {
		b = in[i];
		for (j = 0; j < 8; j++) {
			SD_CLK_LOW;_sync();
			if (b & 0x80) {
				SD_CMD_HIGH;_sync();
			} else {
				SD_CMD_LOW;_sync();
			}
			crc <<= 1;
			SD_CLK_HIGH;_sync();
			if ((crc ^ b) & 0x80) crc ^= 0x09;
			b<<=1;
		}
		crc &= 0x7f;
	}
	crc = ((crc << 1) | 0x01);
	b = crc;
	for (j = 0; j < 8; j++) {
		SD_CLK_LOW;_sync();
		if (crc & 0x80) {
			SD_CMD_HIGH;_sync();
		} else {
			SD_CMD_LOW;_sync();
		}
		SD_CLK_HIGH;_sync();
		crc <<= 1;
	}
	return b;
}
//--------------------------------------------------------
void MDD_SD4BIT_InitIO (void)
{
}

BYTE MDD_SD4BIT_MediaDetect(void)
{
	return MDD_SD4BIT_MediaDetected;
}//end MediaDetect

MEDIA_INFORMATION *  MDD_SD4BIT_MediaInitialize(void)
{
	MEDIA_INFORMATION * result = NULL;
	
	if (mediaInformation.errorCode == MEDIA_NO_ERROR) {
		result = &mediaInformation;
	}
	return result;
}//end MediaInitialize

DWORD MDD_SD4BIT_ReadCapacity(void)
{
	return (MDD_SD4BIT_finalLBA);
}

WORD MDD_SD4BIT_ReadSectorSize(void)
{
	return gMediaSectorSize_SD4BIT;
}

BYTE MDD_SD4BIT_SectorRead(DWORD sector_addr, BYTE* buffer) {
	BYTE result = FALSE;
	if (buffer != NULL) {
		if (SdReadBlock(sector_addr, buffer) == 0) {
			result = TRUE;
		}
	}
	return result;
}

BYTE MDD_SD4BIT_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero) {
	BYTE result = FALSE;
	if (buffer != NULL) {
		if ( ((sector_addr == 0) && (allowWriteToZero == 1)) || ((sector_addr != 0) && (allowWriteToZero == 0)) ) {
			if (SdWriteBlock(sector_addr, buffer) == 0) {
				result = TRUE;
			}
		}	
	}
	return result;
}

BYTE MDD_SD4BIT_WriteProtectState(void)
{
	return(0);
}

BYTE MDD_SD4BIT_ShutdownMedia(void)
{
	return 0;
}

static void do_MDD_SD4BIT_MediaDetect(void) {
	//First check if 4BIT module is enabled or not.
	if (MDD_SD4BIT_MediaDetect() == 0)
	{
		BYTE result = SdCardInit();
		if (result == 0) {
			/* if the card was initialized correctly, it means it is present */
			mediaInformation.errorCode = MEDIA_NO_ERROR;
			mediaInformation.validityFlags.value = 0;
			gMediaSectorSize_SD4BIT = 512u;
			mediaInformation.sectorSize = 512u;
			MDD_SD4BIT_MediaDetected = 1;
		}
	}
}
