#include "dee.h"
#include "c_dee.h"
#include "int_dee.h"

#include "k_stdtype.h"
#include "mal.h"

unsigned int NVMErasePage(void* address);
unsigned int NVMWriteWord(void* address, unsigned int data);

typedef struct _Dee_Type {
	Dee_Page_Status status;
	uint32 data[DEE_NUMBER_OF_DATA_ELEMENTS];
	uint32 crc[DEE_NUMBER_OF_DATA_ELEMENTS];
	uint32 address[DEE_NUMBER_OF_DATA_ELEMENTS];
} Dee_Type;

typedef union _Dee_Page {
	uint32 pages[DEE_PAGE_PAGE_SIZE][NUMBER_OF_INSTRUCTIONS_IN_PAGE];
	uint32 raw[NUMBER_OF_INSTRUCTIONS_IN_PAGE * DEE_PAGE_PAGE_SIZE];
	Dee_Type type;
} Dee_Page;

const Dee_Page dee_page[DEE_PAGE_CNT] __attribute__ (( address(EEPROM_FLASH_ADDR), aligned(NUMBER_OF_BYTES_IN_PAGE) )) = {0};
Dee_Page *read_dee_page;
Dee_Page_Status_Helper dee_debug_page_status[DEE_PAGE_CNT];

void init_dee_page(void) {
    read_dee_page = (Dee_Page *)KVA0_TO_KVA1(dee_page);
	dee_page_update_debug_status();
}

void do_dee_page(void) {
	dee_page_update_debug_status();
}

uint32 GetEraseCounter(uint8 page) {
	uint32 result = 0;
	result = read_dee_page[page].type.status.page_erase_count;//read page status completly
	return result;
}

void dee_page_update_debug_status(void) {
	if (dee_debug_enable) {
		uint8 i = 0;
		for (i = 0; i < DEE_PAGE_CNT; i++) {
			dee_debug_page_status[i].status = read_dee_page[i].type.status;
		}
	}
}

uint32 dee_page_mass_erase_page(uint32 page) {
	uint32 result = 0;
	uint32 i = 0;
	for (i = 0; i < DEE_PAGE_PAGE_SIZE; i ++ ) {
		result = NVMErasePage((void*)dee_page[page].pages[i]);
		if (result) {
			break;
		}
	}
	return result;
}

uint32 dee_page_erase_page(uint32 page, uint32 eraseCounter) {
	uint32 result = 0;
	uint32 i = 0;
	uint8 wasClearPagesOk = 1;
	for (i = 0; i < DEE_PAGE_PAGE_SIZE; i ++ ) {
		result = NVMErasePage((void*)dee_page[page].pages[i]);
		if (result) {
			wasClearPagesOk = 0;
			break;
		}
	}
	if (wasClearPagesOk) {
		result = SetPageStatus(page, STATUS_INC_ERASE, 0xFF, 0xFF, 0xFF, eraseCounter + 1);
	}
	return result;
}

uint32 dee_page_read(uint32 page, uint32 row) {
	uint32 result = 0;
	result = read_dee_page[page].raw[row];
	return result;
}

Dee_Page_Status dee_page_read_status(uint32 page) {
	Dee_Page_Status result = {0};
	result = read_dee_page[page].type.status;
	return result;
}

uint32 dee_page_read_address(uint32 page, uint32 element) {
	uint32 result = 0;
	result = read_dee_page[page].type.address[element];
	return result;
}

uint32 dee_page_read_data(uint32 page, uint32 element) {
	uint32 result = 0;
	result = read_dee_page[page].type.data[element];
	return result;
}

uint32 dee_page_read_crc(uint32 page, uint32 element) {
	uint32 result = 0;
	result = read_dee_page[page].type.crc[element];
	return result;
}

uint32 dee_page_write_status(uint32 page, uint32 status) {
	uint32 result = 0;
	result = NVMWriteWord((void*)&dee_page[page].type.status, status);
	return result;
}

uint32 dee_page_write_address(uint32 page, uint32 element, uint32 value) {
	uint32 result = 0;
	result = NVMWriteWord((void*)(&dee_page[page].type.address[element]), value); 
	return result;
}

uint32 dee_page_write_data(uint32 page, uint32 element, uint32 value) {
	uint32 result = 0;
	result = NVMWriteWord((void*)(&dee_page[page].type.data[element]), value); 
	return result;
}

uint32 dee_page_write_crc(uint32 page, uint32 element, uint32 value) {
	uint32 result = 0;
	result = NVMWriteWord((void*)(&dee_page[page].type.crc[element]), value); 
	return result;
}

void dee_page_read_element(uint8 page, uint32 element, uint32 *data, uint32 *crc, uint32 *address) {
	if (page < DEE_PAGE_CNT) {
		if (element < DEE_NUMBER_OF_DATA_ELEMENTS) {
			if (address != NULL) {
				*address = dee_page_read_address(page, (DEE_NUMBER_OF_DATA_ELEMENTS - 1) - element);
			}
			if (data != NULL) {
				*data = dee_page_read_data(page, element);
			}
			if (crc != NULL) {
				*crc = dee_page_read_crc(page, element);
			}
		}
	}
}

uint32 dee_page_write_element(uint8 page, uint32 element, uint32 data, uint32 crc, uint32 address) {
	uint32 retCode = 0;
	if (page < DEE_PAGE_CNT) {
		if (element < DEE_NUMBER_OF_DATA_ELEMENTS) {
			retCode = dee_page_write_address(page, (DEE_NUMBER_OF_DATA_ELEMENTS - 1) - element, address); 
			if (!retCode) {
				retCode = dee_page_write_data(page, element, data);
			}
			if (!retCode) {
				retCode = dee_page_write_crc(page, element, crc);
			}

			if (retCode & _NVMCON_LVDERR_MASK) {
				SetLowVoltageError(1);
				return (8);
			}
			else if (retCode & _NVMCON_WRERR_MASK) {
				SetPageWriteError(1);
				return (7);
			}
		} else {
			SetWrongElementNumber(1);
			return (12);
		}
	} else {
		SetWrongPage(1);
		return (10);
	}
	return retCode;
}
