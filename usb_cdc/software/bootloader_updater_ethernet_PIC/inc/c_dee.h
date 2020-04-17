#ifndef _C_DEE_H_
#define _C_DEE_H_

//Size of the eepom in int (32bits). Address range is then from 0 to DEE_EEPROM_SIZE - 1
//Actualy no relation to the actual used page. Since this means which addresranges can be stored.
//It is advised that you use at least so much bytes for one eeprom page that is defined here
#define DEE_EEPROM_SIZE 4096

//Defines how much page should be created. Normally we operate with pages, and when one is full we move everyting to the next one.
//More pages means more write cycles. But more pages does not mean that eeprom can be configured bigger (DEE_EEPROM_SIZE)
//Minimum are 3 pages
#define DEE_PAGE_CNT 3

//Defines that one eeprom page how much flash pages should use.
#define DEE_PAGE_PAGE_SIZE	3

#define EEPROM_FLASH_ADDR 0x9D030000

#endif
