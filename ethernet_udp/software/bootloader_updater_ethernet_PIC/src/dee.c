#include "dee.h"
#include "c_dee.h"
#include "int_dee.h"

#include "k_stdtype.h"
#include "mal.h"

#define NVMCON_NVMOP           	0x0000000f
#define NVMCON_ERASE            0x00000040
#define NVMCON_WRERR            0x00002000
#define NVMCON_WREN             0x00004000
#define NVMCON_WR               0x00008000

#define NVMCON_NVMOP0           0x00000001
#define NVMCON_NVMOP1           0x00000002
#define NVMCON_NVMOP2           0x00000004
#define NVMCON_NVMOP3           0x00000008

#define NVMCON_PROGOP           0x0000000F

#define NVMCON_PROGOP0          0x00000001
#define NVMCON_PROGOP1          0x00000002
#define NVMCON_PROGOP2          0x00000004
#define NVMCON_PROGOP3          0x00000008 

#define ERASE_WRITE_CYCLE_MAX           (1000) // Maximum erase cycle per page

DATA_EE_FLAGS dataEEFlags;         //Flags for the error/warning condition. 
volatile int dee_debug_last_return_value = 0;
uint32 GetIndexOfEmptyDataElement_LastFoundHere [DEE_PAGE_CNT] = {0};
uint8 dee_cache_valid = 0;//used to store the data for the last read access. 0 - invalid  1 - valid
uint32 dee_data_cache = 0;
uint32 dee_addr_cache = 0;

uint8 dee_debug_enable = 1;
uint8 do_dee_debug = 0;
uint32 dee_debug_init_dee = 0;

sint32 GetIndexOfEmptyDataElement(uint8 page);
uint32 ErasePage(uint8 page);
uint32 MassErasePage(uint8 page);
uint32 DataEEWrite(uint32 data, uint32 addr, uint8 packOperation);
uint32 DataEERead(uint32 *data, uint32 addr);
uint32 PackEE(void);
uint32 Dee_CloseCurrentPage(uint8 currentPage, uint8 nextPage);
unsigned int NVMWriteWord(void* address, unsigned int data);
unsigned int NVMErasePage(void* address);
unsigned int __attribute__((nomips16)) _NVMOperation(unsigned int nvmop);

void init_dee(void) {
    uint8 activePage = 0;
	uint32 i = 0;
    uint8 currentPage = 0xFF;
    uint8 nextPage = 0xFF;
    uint32 retCode = 0;

	init_dee_page();
	dee_cache_valid = 0;
    
    dataEEFlags.val = 0;
    //Erase the whlole emulation page for the first time
    for (i = 0; i < DEE_PAGE_CNT; i++) {
        uint32 tempReadByte = 0xFFFFFFFF;
        tempReadByte = dee_page_read(i, 0);
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
	activePage = GetActiveCurrentPageCount();

    //If no active pages found, initialize page 0
    if(activePage == 0) {
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
    } else if (activePage == DEE_PAGE_CNT) {
		//If Full active pages, erase the page after the current page
        ErasePage(nextPage); // Erase the page after the current page

        if (GetIndexOfEmptyDataElement(currentPage) == -1) {//Page full
            PackEE();
        }
    } else if (activePage > 0) {//If some active pages, do nothing
    } else {
        SetPageCorruptStatus(1);
        dee_debug_init_dee = 6;
    }
}

void deinit_dee(void) {
}

void do_dee(void) {
	if (do_dee_debug) {
		do_dee_debug = 0;
		static uint8 debugCnt = 0;
		extern void write_eeprom(unsigned int addr, unsigned char data);
		//write_eeprom(2, debugCnt);
		debugCnt++;
	}
	do_dee_page();
}

void isr_dee_1ms(void) {
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
    uint8 activePage = 0;
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
	activePage = GetActiveCurrentPageCount();

    if (activePage == 0) {
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
		if ( (addrIndex == (DEE_NUMBER_OF_DATA_ELEMENTS - 1)) && (activePage < (DEE_PAGE_CNT - 1) ) ) {
			Dee_CloseCurrentPage(currentPage, nextPage);
		} else if ( (addrIndex == (DEE_NUMBER_OF_DATA_ELEMENTS - 1)) && (activePage == (DEE_PAGE_CNT - 1) ) ) { // both active pages are full then pack the page.
			PackEE();
		}
	} else {
		if ( (addrIndex == (DEE_NUMBER_OF_DATA_ELEMENTS - 1)) && (activePage < (DEE_PAGE_CNT - 1) ) ) {
			//Page is full, but there is no other page since we are already in Pack operation.
			//Possible deadlock
		}
	}
    return(0);
}

uint32 DataEERead(uint32 *data, uint32 addr) {
    uint8 currentPage = 0xFF;
    uint8 previousPage = 0xFF;
    uint32 activePage = 0;

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
	activePage = GetActiveCurrentPageCount();


    if (activePage == 0) {
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
			} else if ((dataFound == 0) && (activePage == 2)) {//look in previous page if that is also active
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

uint32 PackEE(void) {
//Possible optimization is to do this asynch. But then read and write operation must be also asynch
//or application must check it read and write is allowed.
    uint8 previousPage = 0xFF;
    uint8 currentPage = 0xFF;
	uint8 nextPage = 0xFF;
    int activePage = 0;

	currentPage = GetActiveCurrentPage();
	if (currentPage == 0xFF) {
		SetNoActiveCurrentPage(1);
		return (9);
	}
	previousPage = GetPreviousPage(currentPage);
	nextPage = GetNextPage(currentPage);
	if (currentPage == 0xFF) {
		SetNoActiveCurrentPage(1);
		return (9);
	}
	activePage = GetActiveCurrentPageCount();

	switch(activePage) {
		case 1 : {
			break;
		}
		case 2 : {
			uint32 retCode = 0;
			uint32 data = 0;
			uint32 addr = 0;
			for (addr = 0; addr < DEE_EEPROM_SIZE; addr++) {
				if (DataEERead(&data, addr) == 0) {
					if (DataEEWrite(data, addr, 1) == 0) {
					} else if (dataEEFlags.val == 1) {
						//address not found. proceed to next element
					} else if (dataEEFlags.val > 1) {
						SetPageCorruptStatus(1);
						return (6);
					}
				} else if (dataEEFlags.val == 1) {
					//address not found. proceed to next element
				} else if (dataEEFlags.val > 1) {
					SetPageCorruptStatus(1);
					return (6);
				}
			}
			
			if (GetIndexOfEmptyDataElement(nextPage) != -1) {
				retCode = SetPageStatus(nextPage, STATUS_ACTIVE, 0xFF, 0xFF, 0xFF, 0); //mark the packed page as active and current.
				ErasePage(currentPage);
				ErasePage(previousPage);
			} else {
				retCode = SetPageStatus(nextPage, STATUS_ACTIVE, STATUS_CURRENT, 0xFF, 0xFF, 0); //mark the packed page as active and not current.
				ErasePage(currentPage);
				ErasePage(previousPage);
				retCode = SetPageStatus(GetNextPage(nextPage), STATUS_ACTIVE, 0xFF, 0xFF, 0xFF, 0);
			}

			if (retCode & _NVMCON_LVDERR_MASK) {
				SetLowVoltageError(1);
				return (8);
			} else if (retCode & _NVMCON_WRERR_MASK) {
				SetPageWriteError(1);
				return (7);
			}
			break;
		}
		case 3 : {
			SetPagePackBeforeInit(1);
			break; // Error - no active page
		}
		default : {
			break;
		}
    }
    return(0);
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
