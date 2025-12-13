#include "dee.h"
#include "c_dee.h"
#include "int_dee.h"

#include "k_stdtype.h"
#include "mal.h"
#include "eeprom.h"

#ifdef DEE_TESTING_ENABLED
	#include "tmr.h"
#endif

#define NVMCON_NVMOP			0x0000000f
#define NVMCON_ERASE			0x00000040
#define NVMCON_WRERR			0x00002000
#define NVMCON_WREN				0x00004000
#define NVMCON_WR				0x00008000

#define NVMCON_NVMOP0			0x00000001
#define NVMCON_NVMOP1			0x00000002
#define NVMCON_NVMOP2			0x00000004
#define NVMCON_NVMOP3			0x00000008

#define NVMCON_PROGOP			0x0000000F

#define NVMCON_PROGOP0			0x00000001
#define NVMCON_PROGOP1			0x00000002
#define NVMCON_PROGOP2			0x00000004
#define NVMCON_PROGOP3			0x00000008 

#define ERASE_WRITE_CYCLE_MAX	(1000) // Maximum erase cycle per page

typedef enum _PackEE_States {
	PackEE_States_Start = 0,
	PackEE_States_ForCycle,
	PackEE_States_FinalCheck,
	PackEE_States_FinalCheck_Erase_1a,
	PackEE_States_FinalCheck_Erase_1b,
	PackEE_States_FinalCheck_Erase_2a,
	PackEE_States_FinalCheck_Erase_2b,
	PackEE_States_FinalCheck_After_Erase,
	PackEE_States_Stop,
} PackEE_States;

DATA_EE_FLAGS dataEEFlags;		 //Flags for the error/warning condition. 
volatile int dee_debug_last_return_value = 0;
uint32 GetIndexOfEmptyDataElement_LastFoundHere [DEE_PAGE_CNT] = {0};
uint8 dee_cache_valid = 0;//used to store the data for the last read access. 0 - invalid  1 - valid
uint32 dee_data_cache = 0;
uint32 dee_addr_cache = 0;

uint8 dee_debug_enable = 1;
volatile uint8 do_dee_debug = 0;
uint32 dee_debug_init_dee = 0;

uint8 dee_pack_running = 0;
uint8 dee_pack_triggered = 0;
volatile uint32 do_dee_1ms = 0;
volatile PackEE_States PackEE_async_state_prev = PackEE_States_Start;

sint32 GetIndexOfEmptyDataElement(uint8 page);
uint32 ErasePage(uint8 page);
uint32 MassErasePage(uint8 page);
uint32 DataEEWrite(uint32 data, uint32 addr, uint8 packOperation);
uint32 DataEERead(uint32 *data, uint32 addr);
uint32 PackEE(void);
sint32 PackEE_async(void);
uint32 Dee_CloseCurrentPage(uint8 currentPage, uint8 nextPage);
unsigned int NVMWriteWord(void* address, unsigned int data);
unsigned int NVMErasePage(void* address);
unsigned int __attribute__((nomips16)) _NVMOperation(unsigned int nvmop);

#ifdef DEE_TESTING_ENABLED
	#define DEE_TEST_NOT_RUN 0xFF
	#define DEE_TEST_FAILED 0x00
	#define DEE_TEST_PASSED 0x01
	#define DEE_TEST_RUNNING 0x03

	volatile uint8 test_dee_TC_result[] = {DEE_TEST_NOT_RUN, DEE_TEST_NOT_RUN, DEE_TEST_NOT_RUN};
	volatile uint32 test_debug_var = 0;
	extern uint8 test_erase_pages(void);

	void test_write_page_full(void) {
		uint32 written_elements = 0;
		uint8 cnt = 0;
		uint8 activePages = 0;
		while (1) {
			activePages = GetActiveCurrentPageCount();
			if (activePages >= (DEE_PAGE_CNT - 1)) {
				break;
			}
			write_eeprom_char(0x0008, cnt);
			cnt++;
			written_elements++;
		}
	}

	volatile uint32 do_dee_max_runtime = 0x0;
	volatile uint32 do_dee_max_runtime_actual = 0xFFFFFFFF;
	volatile uint32 do_dee_max_runtime_before = 0xFFFFFFFF;
	volatile uint32 do_dee_max_runtime_after = 0xFFFFFFFF;


	volatile uint32 reorg_runtime = 0x0;
	volatile uint32 reorg_runtime_actual = 0xFFFFFFFF;
	volatile uint32 reorg_runtime_before = 0xFFFFFFFF;
	volatile uint32 reorg_runtime_after = 0xFFFFFFFF;

	void test_force_reorganization(void) {
		dee_pack_triggered = 1;

		reorg_runtime_before = getGlobalTime();
		while (dee_pack_triggered) {
			do_dee_max_runtime_before = getGlobalTime();
			do_dee();
			do_dee_max_runtime_after = getGlobalTime();
			do_dee_max_runtime_actual = do_dee_max_runtime_after - do_dee_max_runtime_before;
			if (do_dee_max_runtime_actual > do_dee_max_runtime) {
				do_dee_max_runtime = do_dee_max_runtime_actual;
			}
			isr_dee_1ms();
		}
		reorg_runtime_after = getGlobalTime();
		reorg_runtime_actual = reorg_runtime_after - reorg_runtime_before;
	}

	void test_dee_TC1(void) {
		//TC1
		test_dee_TC_result[0] = DEE_TEST_RUNNING;
		if (test_erase_pages() == 0) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		init_dee();
		if (dee_debug_init_dee != 0) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_debug_var = dee_page_read_raw(0, 0);
		if (dee_page_read_raw(0, 0) != 0x00000017) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_debug_var = dee_page_read_raw(1, 0);
		if (dee_page_read_raw(1, 0) != 0x0000001F) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_debug_var = dee_page_read_raw(2, 0);
		if (dee_page_read_raw(2, 0) != 0x0000001F) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		write_eeprom_char(0x0000, 0xCA);
		write_eeprom_char(0x0001, 0xFE);
		write_eeprom_char(0x0002, 0xDE);
		write_eeprom_char(0x0003, 0xAD);
		write_eeprom_char(0x0004, 0xCC);
		write_eeprom_char(0x0005, 0xCD);
		write_eeprom_char(0x0006, 0xCE);
		write_eeprom_char(0x0007, 0xCF);
		
		if (read_eeprom_char(0x0000) != 0xCA) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0001) != 0xFE) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0002) != 0xDE) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0003) != 0xAD) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}


		test_write_page_full();

		test_debug_var = dee_page_read_raw(0, 0);
		if (dee_page_read_raw(0, 0) != 0x00000015) {//0101
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_debug_var = dee_page_read_raw(1, 0);
		if (dee_page_read_raw(1, 0) != 0x00000017) {//0111
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_debug_var = dee_page_read_raw(2, 0);
		if (dee_page_read_raw(2, 0) != 0x0000001F) {//1111
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_force_reorganization();

		test_debug_var = dee_page_read_raw(0, 0);
		if (dee_page_read_raw(0, 0) != 0x0000002F) {//1111
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_debug_var = dee_page_read_raw(1, 0);
		if (dee_page_read_raw(1, 0) != 0x0000002F) {//1111
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		test_debug_var = dee_page_read_raw(2, 0);
		if (dee_page_read_raw(2, 0) != 0x00000017) {//1111
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}

		
		if (read_eeprom_char(0x0004) != 0xCC) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0005) != 0xCD) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0006) != 0xCE) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0007) != 0xCF) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0000) != 0xCA) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0001) != 0xFE) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0002) != 0xDE) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		if (read_eeprom_char(0x0003) != 0xAD) {
			test_dee_TC_result[0] = DEE_TEST_FAILED;
			return;
		}
		test_dee_TC_result[0] = DEE_TEST_PASSED;
	}

	void test_dee(void) {
		test_dee_TC1();
		
		test_erase_pages();

		while (1);
	}
#endif

void init_dee(void) {
	uint8 activePages = 0;
	uint32 i = 0;
	uint8 currentPage = 0xFF;
	uint8 nextPage = 0xFF;
	uint32 retCode = 0;

	init_eeprom();
	init_dee_page();
	dee_cache_valid = 0;
	
	dataEEFlags.val = 0;
	//Erase the whlole emulation page for the first time
	for (i = 0; i < DEE_PAGE_CNT; i++) {
		uint32 tempReadByte = 0xFFFFFFFF;
		Dee_Page_Status_Helper temp_helper;
		temp_helper.status = dee_page_read_status(i);
		tempReadByte = temp_helper.byte;
		if (tempReadByte == 0x0) {
			retCode = MassErasePage(i);
			if(retCode & _NVMCON_LVDERR_MASK) {
				SetLowVoltageError(1);
				dee_debug_init_dee = 8;
			} else if(retCode & _NVMCON_WRERR_MASK) {
				SetPageWriteError(1);
				dee_debug_init_dee = 7;
			}
		}
	}
	
	{
		// Find unexpired page
		uint8 unexpiredPage = 0xFF;
		unexpiredPage = GetUnexpiredPage();
		if (unexpiredPage != 0xFF) {
			SetPageExpiredPage(1);
			dee_debug_init_dee = 2;
		}
	}
	
	//count active pages
	currentPage = GetActiveCurrentPage();//no error check since activePage will do this later
	nextPage = GetNextPage(currentPage);//no error check since activePage will do this later
	activePages = GetActiveCurrentPageCount();

	//If no active pages found, initialize page 0
	if(activePages == 0) {
		for(i = 0; i < DEE_PAGE_CNT;i++) {
			retCode = SetPageStatus(i, STATUS_ZERO_ERASE, 0xFF, 0xFF, 0xFF, 0);//Set Erase counter to 0
			if(retCode & _NVMCON_LVDERR_MASK) {
				SetLowVoltageError(1);
				dee_debug_init_dee = 8;
			} else if(retCode & _NVMCON_WRERR_MASK) {
				SetPageWriteError(1);
				dee_debug_init_dee = 7;
			}
			ErasePage(i);
		}
		
		retCode = SetPageStatus(0, STATUS_ACTIVE, 0xFF, 0xFF, 0xFF, 0);// Page Active & Current
		if(retCode & _NVMCON_LVDERR_MASK) {
			SetLowVoltageError(1);
			dee_debug_init_dee = 8;
		} else if(retCode & _NVMCON_WRERR_MASK) {
			SetPageWriteError(1);
			dee_debug_init_dee = 7;
		}
	} else if (activePages == DEE_PAGE_CNT) {
		//If Full active pages, erase the page after the current page
		ErasePage(nextPage); // Erase the page after the current page

		if (GetIndexOfEmptyDataElement(currentPage) == -1) {//Page full
			dee_pack_triggered = 1;
		}
	} else if (activePages > 0) {//If some active pages, do nothing
	} else {
		SetPageCorruptStatus(1);
		dee_debug_init_dee = 6;
	}
}

void deinit_dee(void) {
	deinit_eeprom();
}

void do_dee(void) {
	do_eeprom();
	if (do_dee_debug) {
		do_dee_debug = 0;
		static uint8 debugCnt = 0;
		extern void write_eeprom(unsigned int addr, unsigned char data);
		//write_eeprom(2, debugCnt);
		debugCnt++;
	}
	
	if (do_dee_1ms) {
		do_dee_1ms = 0;
		if (dee_pack_triggered) {
			sint32 PackEE_async_result = -1;
			PackEE_async_result = PackEE_async();
			if (PackEE_async_result != -1) {
				dee_pack_triggered = 0;
			}
		}
	}
	
	do_dee_page();
}

void isr_dee_1ms(void) {
	isr_eeprom_1ms();
	do_dee_1ms = 1;
	do_dee_debug = 1;
}

void dee_write_eeprom(uint32 addr, uint32 data) {
	if (addr < DEE_EEPROM_SIZE) {
		uint32 retVal = DataEEWrite(data, addr, 0);
		dee_data_cache = data;
		dee_addr_cache = addr;
		dee_cache_valid = 1;
		dee_debug_last_return_value = retVal;
	}
}

uint32 dee_read_eeprom(uint32 addr) {
	uint32 result = 0xFFFFFFFF;
	if (addr < DEE_EEPROM_SIZE) {
		if ((dee_cache_valid == 1) && (dee_addr_cache == addr)) {
			result = dee_data_cache;
		} else {
			uint32 tempData = 0;
			uint32 retVal = DataEERead(&tempData, addr);
			dee_debug_last_return_value = retVal;
			if (retVal == 0) {
				dee_addr_cache = addr;
				dee_data_cache = tempData;
				dee_cache_valid = 1;
				result = tempData;
			}
		}
	}
	return result;
}

sint32 GetIndexOfEmptyDataElement(uint8 page) {
	//This could be optimized so that not the whole eeprom must be read to find the free space
	sint32 nextAvailItem = -1;

	if (page < DEE_PAGE_CNT) {
		uint32 i = 0;
		uint32 iStart = 0;
		uint32 address = 0;
		uint32 data = 0;
		uint32 crc = 0;
		nextAvailItem = -1;
		if (GetIndexOfEmptyDataElement_LastFoundHere[page] < DEE_NUMBER_OF_DATA_ELEMENTS) {
			if (GetIndexOfEmptyDataElement_LastFoundHere[page] > 0) {
				iStart = GetIndexOfEmptyDataElement_LastFoundHere[page] - 1;
			}
		}
		
		for (i = iStart; i < DEE_NUMBER_OF_DATA_ELEMENTS; i ++) {
			dee_page_read_element(page, i, &data, &crc, &address);
			if ((address == 0xFFFFFFFF) && (data == 0xFFFFFFFF) && (crc == 0xFFFFFFFF)) {//empty
				nextAvailItem = i;
				GetIndexOfEmptyDataElement_LastFoundHere[page] = i;
				break;
			}
		}
	}
	return(nextAvailItem);
}

uint32 ErasePage(uint8 page) {
	uint32 eraseCounter = 0;
	uint32 retCode = 0;

	if (page >= DEE_PAGE_CNT) {
		SetWrongPage(1);
		return (10);
	}

	GetIndexOfEmptyDataElement_LastFoundHere[page] = 0;
	eraseCounter = GetEraseCounter(page);
	if ( (eraseCounter + 1) >= ERASE_WRITE_CYCLE_MAX) {
		retCode = SetPageStatus(page, STATUS_EXPIRED, 0xFF, 0xFF, 0xFF, 0);
	} else {
		retCode = dee_page_erase_page(page, eraseCounter);
	}
	
	if (retCode & _NVMCON_LVDERR_MASK) {
		SetLowVoltageError(1);
		return (8);
	} else if (retCode & _NVMCON_WRERR_MASK) {
		SetPageWriteError(1);
		return (7);
	}
	return 0;
}

uint32 MassErasePage(uint8 page) {
	uint32 retCode = 0;

	if (page >= DEE_PAGE_CNT) {
		SetWrongPage(1);
		return (10);
	}

	GetIndexOfEmptyDataElement_LastFoundHere[page] = 0;
	retCode = dee_page_mass_erase_page(page);
	
	if (retCode & _NVMCON_LVDERR_MASK) {
		SetLowVoltageError(1);
		return (8);
	} else if (retCode & _NVMCON_WRERR_MASK) {
		SetPageWriteError(1);
		return (7);
	}
	return 0;
}

uint32 DataEEWrite(uint32 data, uint32 addr, uint8 packOperation) {
	uint8 currentPage = 0xFF;
	uint8 nextPage = 0xFF;
	uint8 activePages = 0;
	sint32 addrIndex = -1;
	uint8 writeToNextPage = 0;

	if (addr >= DEE_EEPROM_SIZE) {
		SetPageIllegalAddress(1);
		return(5);
	}

	currentPage = GetActiveCurrentPage();
	nextPage = GetNextPage(currentPage);
	if (currentPage == 0xFF) {
		return 9;
	}
	if (packOperation) {
		currentPage = GetNextPage(currentPage);
		if (currentPage == 0xFF) {
			return 9;
		}
	}
	activePages = GetActiveCurrentPageCount();

	if (activePages == 0) {
		SetPageCorruptStatus(1);
		return (6);
	}
	//Do not write data if it did not change
	if (packOperation == 0) {//Skip this check if nextPage write is used, reason is PackEE always tries to write the same data
		uint32 dataRead = 0;
		if ( (DataEERead(&dataRead, addr) != 0) && (dataEEFlags.val > 1) ) {
			return (6); //error condition
		}
		if (dataRead == data) {
			return(0);
		}
	}
	
	addrIndex = GetIndexOfEmptyDataElement(currentPage);
	if (addrIndex == -1) { //Page Full
		Dee_CloseCurrentPage(currentPage, nextPage);
		addrIndex = GetIndexOfEmptyDataElement(nextPage);
		writeToNextPage = 1;
		if (addrIndex == -1) { //Page Full
			SetPagePackSkipped(1);
			return(4);  //Error - Number of writes exceeds page size
		}
	}

	if (writeToNextPage == 0) {
		uint32 retCode = 0;
		uint32 data_written = 0;
		uint32 crc_written = 0;
		uint32 address_written = 0;
		retCode = dee_page_write_element(currentPage, addrIndex, data, ~data, addr);
		if (retCode & _NVMCON_LVDERR_MASK) {
			SetLowVoltageError(1);
			return (8);
		}
		else if (retCode & _NVMCON_WRERR_MASK) {
			SetPageWriteError(1);
			return (7);
		}
		dee_page_read_element(currentPage, addrIndex, &data_written, &crc_written, &address_written);
		//Check whether data and address are written correctly.
		if (
			(address_written != addr) ||
			(data_written != data) ||
			(crc_written != ~data)
		) {
			SetPageWriteError(1);
			return(7);  //Error - RAM does not match PM
		}
	} else {
		uint32 retCode = 0;
		uint32 data_written = 0;
		uint32 crc_written = 0;
		uint32 address_written = 0;
		retCode = dee_page_write_element(nextPage, addrIndex, data, ~data, addr);
		if (retCode & _NVMCON_LVDERR_MASK) {
			SetLowVoltageError(1);
			return (8);
		}
		else if (retCode & _NVMCON_WRERR_MASK) {
			SetPageWriteError(1);
			return (7);
		}
		dee_page_read_element(nextPage, addrIndex, &data_written, &crc_written, &address_written);
		//Check whether data and address are written correctly.
		if (
			(address_written != addr) ||
			(data_written != data) ||
			(crc_written != ~data)
		) {
			SetPageWriteError(1);
			return(7);  //Error - RAM does not match PM
		}
	}

	//Pack if page is full
	if (packOperation == 0) {//Skip this check if nextPage write is used, reason is PackEE always tries to write the same data
		if ( (addrIndex == (DEE_NUMBER_OF_DATA_ELEMENTS - 1)) && (activePages < (DEE_PAGE_CNT - 1) ) ) {
			Dee_CloseCurrentPage(currentPage, nextPage); 
		} else if ( (addrIndex >= (DEE_NUMBER_OF_DATA_ELEMENTS * 80 / 100)) && (activePages == (DEE_PAGE_CNT - 1) ) ) { // both active pages are full then pack the page.
			dee_pack_triggered = 1;
		} else if ( (addrIndex == (DEE_NUMBER_OF_DATA_ELEMENTS - 1)) && (activePages == (DEE_PAGE_CNT - 1) ) ) { // both active pages are full then pack the page.
			dee_pack_triggered = 1;
		}
	} else {
		if ( (addrIndex == (DEE_NUMBER_OF_DATA_ELEMENTS - 1)) && (activePages < (DEE_PAGE_CNT - 1) ) ) {
			//Page is full, but there is no other page since we are already in Pack operation.
			//Possible deadlock
		}
	}
	return(0);
}

uint32 DataEERead(uint32 *data, uint32 addr) {
	uint8 currentPage = 0xFF;
	uint8 previousPage = 0xFF;
	uint32 activePages = 0;

	if (addr >= DEE_EEPROM_SIZE) {
		SetPageIllegalAddress(1);
		return(5);
	}

	if (data == NULL) {
		SetNullPointerError(1);
		return (11);
	}

	currentPage = GetActiveCurrentPage();
	if (currentPage == 0xFF) {
		return 9;
	}
	previousPage = GetPreviousPage(currentPage);
	activePages = GetActiveCurrentPageCount();


	if (activePages == 0) {
	   SetPageCorruptStatus(1);
	   return(6);
	}

	{
		sint32 i = 0;
		uint8 dataFound = 0;
		sint32 addrIndex = -1;
		addrIndex = GetIndexOfEmptyDataElement(currentPage);//Returned itemindex is pointing to the next free item, so one -1 is the first element that is already writte.
		if (addrIndex == 0) {//Page is empty, nothing to search
		} else {
			if (addrIndex == -1) {//Page is full, every element must be searched
				addrIndex = DEE_NUMBER_OF_DATA_ELEMENTS - 1;
			} else {
				addrIndex -= 1;
			}
			for (i = addrIndex; i >= 0; i--) {
				uint32 address = 0;
				dee_page_read_element(currentPage, i, NULL, NULL, &address);
				if (address == addr) {//address found
					uint32 dataTemp = 0;
					uint32 crc = 0;
					dee_page_read_element(currentPage, i, &dataTemp, &crc, NULL);
					if (dataTemp == ~crc) {//check CRC if it is good
						*data = dataTemp;//CRC is ok use data
						dataFound = 1;
						break; //no need to search further, this is the newest element
					} else {
						SetPageCorruptStatus(1); //CRC is not ok, try look for older data
					}
				} else if (address == 0xFFFFFFFF) {//Address is empty so no more entries
					//should not come here at all
				}
			}
		}
		if (dataFound == 0) {//Try old search mechanism
			for (i = 0; i < DEE_NUMBER_OF_DATA_ELEMENTS; i++) {
				//Possible optimization if search does not start with the oldest data, but with the newest
				//Need to get where the last data stored, then search back until the first hit.
				uint32 address = 0;
				dee_page_read_element(currentPage, i, NULL, NULL, &address);
				if (address == addr) {//address found
					uint32 dataTemp = 0;
					uint32 crc = 0;
					dee_page_read_element(currentPage, i, &dataTemp, &crc, NULL);
					if (dataTemp == ~crc) {//check CRC if it is good
						*data = dataTemp;//CRC is ok use data
						dataFound = 1;
						//need to search further, maybe this was only an old element
					} else {
						SetPageCorruptStatus(1); //CRC is not ok, try look for older data
					}
				} else if (address == 0xFFFFFFFF) {//Address is empty so no more entries
					break;
				}
			}
			if (dataFound == 1) {
				return (0); //Success
			} else if ((dataFound == 0) && (activePages > 1)) {//look in previous page if that is also active
				for (i = 0; i < DEE_NUMBER_OF_DATA_ELEMENTS; i++) {
					uint32 address = 0;
					dee_page_read_element(previousPage, i, NULL, NULL, &address);
					if (address == addr) {//address found
						uint32 dataTemp = 0;
						uint32 crc = 0;
						dee_page_read_element(previousPage, i, &dataTemp, &crc, NULL);
						if (dataTemp == ~crc) {//check CRC if it is good
							*data = dataTemp;
							dataFound = 1;
							//need to search further, maybe this was only an old element
						}
					}
				}
				if (dataFound == 1) {
					return (0); //Success
				} else {
					SetaddrNotFound(1);
					return(1);
				}
			} else {
				SetaddrNotFound(1);
				return(1);
			}
		}
	}
	return (0);
}

uint8 dee_is_pack_running(void) {
	unsigned char result = 1;
	
	result = dee_pack_running;
	
	return result;
}

sint32 PackEE_async(void) {
	sint32 result = -1;//Busy
	static uint32 internal_result = 0;
	static PackEE_States PackEE_async_state = PackEE_States_Start;
	static uint8 previousPage = 0xFF;
	static uint8 currentPage = 0xFF;
	static uint8 nextPage = 0xFF;
	static uint32 addr_for = 0;
	static uint32 retCode = 0;

	PackEE_async_state_prev = PackEE_async_state;
	switch (PackEE_async_state) {
		case PackEE_States_Start : {
			{
				previousPage = 0xFF;
				currentPage = 0xFF;
				nextPage = 0xFF;
				int activePages = 0;

				dee_pack_running = 1;
				internal_result = 0;

				currentPage = GetActiveCurrentPage();
				if (currentPage == 0xFF) {
					SetNoActiveCurrentPage(1);
					internal_result = 9;
					PackEE_async_state = PackEE_States_Stop;
				} else {
					previousPage = GetPreviousPage(currentPage);
					nextPage = GetNextPage(currentPage);
					activePages = GetActiveCurrentPageCount();

					if (activePages == (DEE_PAGE_CNT - 1)) {
						addr_for = 0;
						PackEE_async_state = PackEE_States_ForCycle;
					} else if (activePages == DEE_PAGE_CNT) {
						SetPagePackBeforeInit(1);
						PackEE_async_state = PackEE_States_Stop;
					} else {
						PackEE_async_state = PackEE_States_Stop;
					}
				}
			}
			break;
		}
		case PackEE_States_ForCycle : {
			uint32 data = 0;
			if (addr_for < DEE_EEPROM_SIZE) {
				if (DataEERead(&data, addr_for) == 0) {
					if (DataEEWrite(data, addr_for, 1) == 0) {
					} else if (dataEEFlags.val == 1) {
						//address not found. proceed to next element
					} else if (dataEEFlags.val > 1) {
						SetPageCorruptStatus(1);
						internal_result = 6;
						PackEE_async_state = PackEE_States_Stop;
					}
				} else if (dataEEFlags.val == 1) {
					//address not found. proceed to next element
				} else if (dataEEFlags.val > 1) {
					SetPageCorruptStatus(1);

					internal_result = 6;
					PackEE_async_state = PackEE_States_Stop;
				}
				addr_for++;
			} else {
				if (internal_result == 0) {
					PackEE_async_state = PackEE_States_FinalCheck;
				}
			}
			break;
		}
		case PackEE_States_FinalCheck : {
			if (GetIndexOfEmptyDataElement(nextPage) != -1) {
				retCode = SetPageStatus(nextPage, STATUS_ACTIVE, 0xFF, 0xFF, 0xFF, 0); //mark the packed page as active and current.
				PackEE_async_state = PackEE_States_FinalCheck_Erase_1a;
			} else {
				retCode = SetPageStatus(nextPage, STATUS_ACTIVE, STATUS_CURRENT, 0xFF, 0xFF, 0); //mark the packed page as active and not current.
				PackEE_async_state = PackEE_States_FinalCheck_Erase_2a;
			}
			break;
		}
		case PackEE_States_FinalCheck_Erase_1a : {
			ErasePage(currentPage);
			PackEE_async_state = PackEE_States_FinalCheck_Erase_1b;
			break;
		}
		case PackEE_States_FinalCheck_Erase_1b : {
			ErasePage(previousPage);
			PackEE_async_state = PackEE_States_FinalCheck_After_Erase;
			break;
		}
		case PackEE_States_FinalCheck_Erase_2a : {
			ErasePage(currentPage);
			PackEE_async_state = PackEE_States_FinalCheck_Erase_2b;
			break;
		}
		case PackEE_States_FinalCheck_Erase_2b : {
			ErasePage(previousPage);
			retCode = SetPageStatus(GetNextPage(nextPage), STATUS_ACTIVE, 0xFF, 0xFF, 0xFF, 0);
			PackEE_async_state = PackEE_States_FinalCheck_After_Erase;
			break;
		}
		case PackEE_States_FinalCheck_After_Erase : {
			if (retCode & _NVMCON_LVDERR_MASK) {
				SetLowVoltageError(1);
				internal_result = 8;
				PackEE_async_state = PackEE_States_Stop;
			} else if (retCode & _NVMCON_WRERR_MASK) {
				SetPageWriteError(1);
				internal_result = 7;
				PackEE_async_state = PackEE_States_Stop;
			} else {
				PackEE_async_state = PackEE_States_Stop;
			}
			break;
		}
		case PackEE_States_Stop : {
			PackEE_async_state = PackEE_States_Start;
			result = internal_result;
			dee_pack_running = 0;
			break;
		}
		default : {
			result = 0;
			PackEE_async_state = PackEE_States_Start;
			break;
		}
	}

	return result;
}

uint32 Dee_CloseCurrentPage(uint8 currentPage, uint8 nextPage) {
	uint32 retCode = 0;
	//mark the page as not_current and active
	retCode = SetPageStatus(currentPage, STATUS_CURRENT, STATUS_ACTIVE, 0xFF, 0xFF, 0);
	//mark the next page as current and active.
	if (!retCode) {
		retCode = SetPageStatus(nextPage, 0xFF, STATUS_ACTIVE, 0xFF, 0xFF, 0);
	}
	if (retCode & _NVMCON_LVDERR_MASK) {
		SetLowVoltageError(1);
		return (8);
	}
	else if (retCode & _NVMCON_WRERR_MASK) {
		SetPageWriteError(1);
		return (7);
	}
	return retCode;
}

#ifdef __32MZ2048ECG144__
#define NVMOP_WORD_PGM		  0x4001	  // Word program operation
#define NVMOP_PAGE_ERASE		0x4004	  // Page erase operation
#define NVMIsError()	(NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK))

unsigned int NVMWriteWord(void* address, unsigned int data)
{
	unsigned int res;

	NVMADDR = KVA_TO_PA((unsigned int)address);

	// Load data into NVMDATA register
	NVMDATA0 = data;

	// Unlock and Write Word
	res = _NVMOperation(NVMOP_WORD_PGM);

	return res;
}

unsigned int NVMErasePage(void* address)
{
	unsigned int res;

	// Convert Address to Physical Address
	NVMADDR = KVA_TO_PA((unsigned int)address);

	// Unlock and Erase Page
	res = _NVMOperation(NVMOP_PAGE_ERASE);

	// Return WRERR state.
	return res;

}

unsigned int __attribute__((nomips16)) _NVMOperation(unsigned int nvmop)
{
	int	susp;

	// Disable DMA & Disable Interrupts
	#ifdef _DMAC
		lock_isr();
		susp = DmaSuspend();
	#else
		lock_isr(); 
	#endif	// _DMAC

	// Enable Flash Write/Erase Operations
	NVMCON = NVMCON_WREN | nvmop;

	// wait at least 6 us for LVD start-up
	// assume we're running at max frequency
	// (200 MHz) so we're always safe
	{
		unsigned long t0 = _CP0_GET_COUNT();
		while (_CP0_GET_COUNT() - t0 < (200/2)*8);
	}
	NVMKEY = 0;
	NVMKEY 		= 0xAA996655;
	NVMKEY 		= 0x556699AA;
	NVMCONSET 	= NVMCON_WR;

	// Wait for WR bit to clear
	while(NVMCON & NVMCON_WR);

	// Disable Flash Write/Erase operations
	NVMCONCLR = NVMCON_WREN;

	// Enable DMA & Enable Interrupts
	#ifdef _DMAC
		DmaResume(susp);
		unlock_isr();
	#else
		unlock_isr();
	#endif // _DMAC

	// Return Error Status
	return(NVMIsError());
}
#else

#define NVMOP_WORD_PGM		  0x4001	  // Word program operation
#define NVMOP_PAGE_ERASE        0x4004      // Page erase operation
#define NVMIsError()	(NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK))

unsigned int NVMWriteWord(void* address, unsigned int data)
{
    unsigned int res;

    NVMADDR = KVA_TO_PA((unsigned int)address);

    // Load data into NVMDATA register
    NVMDATA = data;

    // Unlock and Write Word
    res = _NVMOperation(NVMOP_WORD_PGM);

	return res;
}

unsigned int NVMErasePage(void* address)
{
    unsigned int res;

    // Convert Address to Physical Address
	NVMADDR = KVA_TO_PA((unsigned int)address);

	// Unlock and Erase Page
	res = _NVMOperation(NVMOP_PAGE_ERASE);

	// Return WRERR state.
	return res;

}

unsigned int __attribute__((nomips16)) _NVMOperation(unsigned int nvmop)
{
    int	susp;

    // Disable DMA & Disable Interrupts
	lock_isr();
	susp = mal_DmaSuspend();

    // Enable Flash Write/Erase Operations
    NVMCON = NVMCON_WREN | nvmop;

    // wait at least 6 us for LVD start-up
    // assume we're running at max frequency
    // (80 MHz) so we're always safe
    {
        unsigned long t0 = _CP0_GET_COUNT();
        while (_CP0_GET_COUNT() - t0 < (80/2)*6);
    }
    
    NVMKEY 		= 0xAA996655;
    NVMKEY 		= 0x556699AA;
    NVMCONSET 	= NVMCON_WR;

    // Wait for WR bit to clear
    while(NVMCON & NVMCON_WR);

    // Disable Flash Write/Erase operations
    NVMCONCLR = NVMCON_WREN;

	// Enable DMA & Enable Interrupts
	mal_DmaResume(susp);
	unlock_isr();

	// Return Error Status
    return(NVMIsError());
}

#endif

//For PIC32MX440F256
//Word Write 20..40us
//Row Write 3..3.5ms
//Page Erase 20ms
//Chip erase 80ms
//Flash LVD Delay 6us