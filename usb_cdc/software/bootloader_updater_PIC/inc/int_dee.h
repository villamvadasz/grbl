#ifndef _INT_DEE_H_
#define _INT_DEE_H_

#define NUMBER_OF_BYTES_IN_PAGE          (4096)
#define NUMBER_OF_INSTRUCTIONS_IN_PAGE  (NUMBER_OF_BYTES_IN_PAGE / 4) // number of 32-bit word instructions per page, 4096 bytes
#define DEE_NUMBER_OF_DATA_ELEMENTS (((NUMBER_OF_INSTRUCTIONS_IN_PAGE * DEE_PAGE_PAGE_SIZE) - (sizeof(Dee_Page_Status) / sizeof(uint32))) / 3)

#define PAGE_CURRENT                    1 // Indicate the page status
#define PAGE_NOT_CURRENT                0 // Indicate the page status
#define PAGE_EXPIRED                    0 // Indicate the page status 
#define PAGE_NOT_EXPIRED                1 // Indicate the page status
#define PAGE_AVAILABLE                  1 // Indicate the page status
#define PAGE_NOT_AVAILABLE              0 // Indicate the page status
#define PAGE_ACTIVE                     0 // Indicate the page status
#define PAGE_NOT_ACTIVE                 1 // Indicate the page status

#define STATUS_ACTIVE                   17 // Indicate the page status
#define STATUS_AVAILABLE                18 // Indicate the page status
#define STATUS_CURRENT                  19 // Indicate the page status
#define STATUS_EXPIRED                  20 // Indicate the page status
#define STATUS_INC_ERASE                21 // Indicate the page status
#define STATUS_ZERO_ERASE				22 
#define STATUS_WRONGE_PAGE				23
#define STATUS_NOT_DEFINE				9999

#define GetaddrNotFound() dataEEFlags.addrNotFound  //Get the flag address not found
#define SetaddrNotFound(x) dataEEFlags.addrNotFound = x // Set the flag address not found

#define GetPageExpiredPage() dataEEFlags.expiredPage  //Get the flag for expired page
#define SetPageExpiredPage(x) dataEEFlags.expiredPage = x // Set the flag for expired page

#define GetPagePackBeforePageFull() dataEEFlags.packBeforePageFull  //Get the flag for pack before page full
#define SetPagePackBeforePageFull(x) dataEEFlags.packBeforePageFull = x //Set the flag for pack before page full

#define GetPagePackBeforeInit() dataEEFlags.packBeforeInit  //Get the flag for pack before initialize
#define SetPagePackBeforeInit(x) dataEEFlags.packBeforeInit = x //Set the flag for pack before initialize

#define GetPagePackSkipped() dataEEFlags.packSkipped //Get the flag pack skipped
#define SetPagePackSkipped(x) dataEEFlags.packSkipped = x //Set the flag pack skipped

#define GetPageIllegalAddress() dataEEFlags.IllegalAddress  //Get the flag for illegal address
#define SetPageIllegalAddress(x) dataEEFlags.IllegalAddress = x //Set the flag for illegal address

#define GetPageCorruptStatus() dataEEFlags.pageCorrupt  //Get the flag page Corrupt
#define SetPageCorruptStatus(x) dataEEFlags.pageCorrupt = x // Set the flag page corrupt
    
#define GetPageWriteError() dataEEFlags.writeError //Get the flag for write error
#define SetPageWriteError(x) dataEEFlags.writeError = x //Set the flag for write error

#define GetLowVoltageError() dataEEFlags.lowVoltageError//Get the flag for low voltage error
#define SetLowVoltageError(x) dataEEFlags.lowVoltageError = x //Set the flag for low voltage error

#define GetNoActiveCurrentPage() dataEEFlags.noActiveCurrentPage
#define SetNoActiveCurrentPage(x) dataEEFlags.noActiveCurrentPage = x

#define GetWrongPage() dataEEFlags.wrongPage
#define SetWrongPage(x) dataEEFlags.wrongPage = x

#define GetNullPointerError() dataEEFlags.nullPointerError
#define SetNullPointerError(x) dataEEFlags.nullPointerError = x

#define GetWrongElementNumber() dataEEFlags.wrongElementNumber
#define SetWrongElementNumber(x) dataEEFlags.wrongElementNumber = x

//After MassErase: 1Not Expired, 1Current,     1Available, 1Not Active 1111
//Activate Page0 : 1Not Expired, 1Current,     1Available, 0Active     1110
//Page0 Full     : 1Not Expired, 0Not Current, 1Available, 1Not Active 1011
//Page1 Open     : 1Not Expired, 0Not Current, 1Available, 0Active     1010

typedef struct _Dee_Page_Status {
	uint32 page_expired : 1;    //1 - not expired 0 - expired
	uint32 page_current : 1;    //1 - current     0 - not current
	uint32 page_available : 1;  //1 - available   0 - not available
	uint32 page_active : 1;     //1 - not active  0 - active
	uint32 page_erase_count :28;
} Dee_Page_Status;

typedef union _Dee_Page_Status_Helper {
	Dee_Page_Status status;
	uint32 byte;
} Dee_Page_Status_Helper;

//Flags for the error/warning conditions.    
typedef union {
	unsigned int val; // a non-zero value indicate error/warning
	struct {
		unsigned addrNotFound:1;       // Return 0x1
		unsigned expiredPage:1;        // Return 0x2
		unsigned packBeforePageFull:1; // Not a return condition
		unsigned packBeforeInit:1;     // Return 0x3
		unsigned packSkipped:1;        // Return 0x4
		unsigned IllegalAddress:1;     // Return 0x5
		unsigned pageCorrupt:1;        // Return 0x6
		unsigned writeError:1;         // Return 0x7
		unsigned lowVoltageError:1;    // Return 0x8
		unsigned noActiveCurrentPage:1;// Return 0x9
		unsigned wrongPage:1;          // Return 0xA
		unsigned nullPointerError:1;   // Return 0xB
		unsigned wrongElementNumber:1;  // Return 0xC
		unsigned reserved:19;          // Reserved for future use
	};
} DATA_EE_FLAGS;

extern uint8 dee_debug_enable;
extern DATA_EE_FLAGS dataEEFlags;


extern void init_dee_page(void);
extern void do_dee_page(void);

extern void dee_page_update_debug_status(void);

extern uint32 dee_page_mass_erase_page(uint32 page);
extern uint32 dee_page_erase_page(uint32 page, uint32 eraseCounter);

extern uint32 dee_page_read(uint32 page, uint32 row);
extern Dee_Page_Status dee_page_read_status(uint32 page);

extern uint32 dee_page_read_address(uint32 page, uint32 element);
extern uint32 dee_page_read_data(uint32 page, uint32 element);
extern uint32 dee_page_read_crc(uint32 page, uint32 element);

extern uint32 dee_page_write_status(uint32 page, uint32 status);
extern uint32 dee_page_write_address(uint32 page, uint32 element, uint32 value);
extern uint32 dee_page_write_data(uint32 page, uint32 element, uint32 value);
extern uint32 dee_page_write_crc(uint32 page, uint32 element, uint32 value);

extern void dee_page_read_element(uint8 page, uint32 element, uint32 *data, uint32 *crc, uint32 *address);
extern uint32 dee_page_write_element(uint8 page, uint32 element, uint32 data, uint32 crc, uint32 address);

extern uint8 GetUnexpiredPage(void);
extern uint8 GetActiveCurrentPage(void);
extern uint8 GetActiveCurrentPageCount(void);
extern uint8 GetNextPage(uint8 page);
extern uint8 GetPreviousPage(uint8 page);

extern uint8 GetPageStatus(uint8 page, uint8 field);
extern uint32 SetPageStatus(uint8 page, uint8 field0, uint8 field1, uint8 field2, uint8 field3, uint32 eraseCounter);
extern uint32 GetEraseCounter(uint8 page);

#endif
