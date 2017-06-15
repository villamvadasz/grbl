#include "dee.h"
#include "c_dee.h"
#include "int_dee.h"

#include "k_stdtype.h"

uint8 GetUnexpiredPage(void) {
	uint8 pageCount = 0;
	uint8 currentPage = 0xFF;
    // Find unexpired page
    for (pageCount = 0; pageCount < DEE_PAGE_CNT; pageCount++) {
		if (GetPageStatus(pageCount, STATUS_EXPIRED) == PAGE_EXPIRED) {
			currentPage = pageCount;
		}
	}
	return currentPage;
}

uint8 GetActiveCurrentPage(void) {
	uint8 pageCount = 0;
	uint8 currentPage = 0xFF;
    // Find the active page.
    for (pageCount = 0; pageCount < DEE_PAGE_CNT; pageCount++) {
		if (GetPageStatus(pageCount, STATUS_ACTIVE) == PAGE_ACTIVE) {
			if (GetPageStatus(pageCount, STATUS_CURRENT) == PAGE_CURRENT) {
				currentPage = pageCount;
			}
		}
	}
	return currentPage;
}

uint8 GetActiveCurrentPageCount(void) {
	uint8 pageCount = 0;
	uint8 activePage = 0;
    // Find the active page.
    for (pageCount = 0; pageCount < DEE_PAGE_CNT; pageCount++) {
		if (GetPageStatus(pageCount, STATUS_ACTIVE) == PAGE_ACTIVE) {
			activePage++;
		}
	}
	return activePage;
}

uint8 GetNextPage(uint8 page) {
	uint8 result = 0xFF;
	if (page == 0xFF) {
		result = 0xFF;
	} else {
		result = (page + 1) % DEE_PAGE_CNT;
	}
	return result;
}

uint8 GetPreviousPage(uint8 page) {
	uint8 result = 0xFF;
	if (page == 0xFF) {
		result = 0xFF;
	} else {	
		result = page;
		result -= 1;
		if (result == 0xFF) {
			result = DEE_PAGE_CNT - 1;
		}
	}
	return result;
}

uint8 GetPageStatus(uint8 page, uint8 field) {
    uint8 status;
    Dee_Page_Status_Helper currentStatus;

    currentStatus.status = dee_page_read_status(page);//read page status completly

    switch(field) {
        case STATUS_ACTIVE: {
            status = currentStatus.status.page_active;
            break;
		}
        case STATUS_CURRENT: {
            status = currentStatus.status.page_current;
            break;
		}
        case STATUS_AVAILABLE: {
            status = currentStatus.status.page_available;
            break;
		}
        case STATUS_EXPIRED: {
            status = currentStatus.status.page_expired;
            break;
		}
        default: {
            status = 0;
            break;
		}
    }
    return(status);
}

uint32 SetPageStatus(uint8 page, uint8 field0, uint8 field1, uint8 field2, uint8 field3, uint32 eraseCounter) {
    uint32 retCode = 0;
    Dee_Page_Status_Helper currentStatus;
	
    currentStatus.status = dee_page_read_status(page);
	
	{
		uint8 x = 0;
		for (x = 0; x < 3; x++) {
			uint8 actualField = 0xFF;
			switch (x) {
				case 0 : {
					actualField = field0;
					break;
				}
				case 1 : {
					actualField = field1;
					break;
				}
				case 2 : {
					actualField = field2;
					break;
				}
				case 3 : {
					actualField = field3;
					break;
				}
				default : {
					actualField = field0;
					break;
				}
			}
			switch (actualField) {
				case STATUS_ACTIVE :  {
					currentStatus.status.page_active = PAGE_ACTIVE;
					break;
				}
				case STATUS_AVAILABLE :  {
					currentStatus.status.page_available = PAGE_NOT_AVAILABLE;
					break;
				}
				case STATUS_CURRENT :  {
					currentStatus.status.page_current = PAGE_NOT_CURRENT;
					break;
				}
				case STATUS_EXPIRED :  {
					currentStatus.status.page_expired = PAGE_EXPIRED;
					break;
				}
				case STATUS_ZERO_ERASE : {
					currentStatus.status.page_erase_count = 0;
					break;
				}
				case STATUS_INC_ERASE : {
					currentStatus.status.page_erase_count = eraseCounter;
					break;
				}
				default : {
					break;
				}
			}
		}
	}

	retCode = dee_page_write_status(page, currentStatus.byte);

	dee_page_update_debug_status();

	return retCode;
}
