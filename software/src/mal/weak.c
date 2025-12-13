#include "k_stdtype.h"

__attribute__(( weak )) void init_iic_master(void) {} 
__attribute__(( weak )) void deinit_iic_master(void) {} 
__attribute__(( weak )) void do_iic_master(void) {} 
__attribute__(( weak )) void isr_iic_master_1ms(void) {} 
__attribute__(( weak )) void isr_iic_master_100us(void) {} 
__attribute__(( weak )) void isr_iic_master_10us(void) {} 
__attribute__(( weak )) void isr_iic_master(void) {} 
	
__attribute__(( weak )) void init_iic_master_multi(void) {} 
__attribute__(( weak )) void deinit_iic_master_multi(void) {} 
__attribute__(( weak )) void do_iic_master_multi(void) {} 
__attribute__(( weak )) void isr_iic_master_multi_1ms(void) {} 
__attribute__(( weak )) void isr_iic_master_multi_100us(void) {} 

__attribute__(( weak )) void init_bme280(void) {} 
__attribute__(( weak )) void deinit_bme280(void) {} 
__attribute__(( weak )) void do_bme280(void) {} 
__attribute__(( weak )) void isr_bme280_1ms(void) {} 

__attribute__(( weak )) void init_dht11(void) {} 
__attribute__(( weak )) void do_dht11(void) {} 
__attribute__(( weak )) void deinit_dht11(void) {} 
__attribute__(( weak )) void isr_dht11_1ms(void) {} 
__attribute__(( weak )) void isr_dht11_external_interrupt(void) {} 

__attribute__(( weak )) void init_ccs811(void) {} 
__attribute__(( weak )) void do_ccs811(void) {} 
__attribute__(( weak )) void deinit_ccs811(void) {} 
__attribute__(( weak )) void isr_ccs811_1ms(void) {} 

__attribute__(( weak )) unsigned char getBME280Temp_update(void) { return 0;}
__attribute__(( weak )) float getBME280Temp(void) { return 0.0; }
__attribute__(( weak )) float getBME280Pressure(void) { return 0.0; }
__attribute__(( weak )) float getBME280Altitude(void) { return 0.0; }
__attribute__(( weak )) float getBME280Humidity(void) { return 0.0; }
__attribute__(( weak )) float getBME280PressureAtSeeLevel(float altitude) { return 0.0; }

__attribute__(( weak )) void init_app_thingspeak_wifi(void) {}
__attribute__(( weak )) void do_app_thingspeak_wifi(void) {}
__attribute__(( weak )) void isr_app_thingspeak_wifi_1ms(void) {}

__attribute__(( weak )) void init_app_thingspeak_lora(void) {}
__attribute__(( weak )) void do_app_thingspeak_lora(void) {}
__attribute__(( weak )) void isr_app_thingspeak_lora_1ms(void) {}

__attribute__(( weak )) void init_wifi(void) {} 
__attribute__(( weak )) void do_wifi(void) {} 
__attribute__(( weak )) void isr_eint_wifi(void) {} 
__attribute__(( weak )) void isr_wifi_1ms(void) {} 

__attribute__(( weak )) void init_serial_usb(void) {}
__attribute__(( weak )) void init_usb(void) {}
__attribute__(( weak )) void do_usb(void) {}
__attribute__(( weak )) void do_serial_usb(void) {}
__attribute__(( weak )) void isr_usb_1ms(void) {}

__attribute__(( weak )) void init_spi_sw(void) {}
__attribute__(( weak )) void deinit_spi_sw(void) {}
__attribute__(( weak )) void do_spi_sw(void) {}
__attribute__(( weak )) void isr_spi_sw_1ms(void) {}

__attribute__(( weak )) void init_sx1276(void) {}
__attribute__(( weak )) void do_sx1276(void) {}
__attribute__(( weak )) void isr_sx1276_100us(void) {}

__attribute__(( weak )) void init_wakeup(void) {}
__attribute__(( weak )) void do_wakeup(void) {}
__attribute__(( weak )) void isr_sx1276_1ms(void) {}

__attribute__(( weak )) void init_sleep(void) {}
__attribute__(( weak )) void do_sleep(void) {}
__attribute__(( weak )) void isr_sleep_1ms(void) {}
__attribute__(( weak )) void deinit_sleep(void) {}
__attribute__(( weak )) unsigned int sleep_stay_in_loop(void) {return 1;}
__attribute__(( weak )) void sleep_after_deinit(void) {}

__attribute__(( weak )) uint8 eep_manager_IsBusy(void) {return 0;}

__attribute__(( weak )) void beforeSleepUser(void) {}
__attribute__(( weak )) void afterSleepUser(void) {}

__attribute__(( weak )) void init_sio(void) {}
__attribute__(( weak )) void do_sio(void) {}
__attribute__(( weak )) void isr_sio_1ms(void) {}

__attribute__(( weak )) void init_random(void) {}
__attribute__(( weak )) void do_random(void) {}
__attribute__(( weak )) void isr_random_1ms(void) {}
