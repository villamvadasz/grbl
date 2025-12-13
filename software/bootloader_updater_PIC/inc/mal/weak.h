#ifndef _WEAK_H_
#define _WEAK_H_

	extern __attribute__(( weak )) void init_iic_master(void); 
	extern __attribute__(( weak )) void deinit_iic_master(void); 
	extern __attribute__(( weak )) void do_iic_master(void); 
	extern __attribute__(( weak )) void isr_iic_master_1ms(void); 
	extern __attribute__(( weak )) void isr_iic_master_100us(void); 
	extern __attribute__(( weak )) void isr_iic_master_10us(void); 
	extern __attribute__(( weak )) void isr_iic_master(void); 
		
	extern __attribute__(( weak )) void init_iic_master_multi(void); 
	extern __attribute__(( weak )) void deinit_iic_master_multi(void); 
	extern __attribute__(( weak )) void do_iic_master_multi(void); 
	extern __attribute__(( weak )) void isr_iic_master_multi_1ms(void); 
	extern __attribute__(( weak )) void isr_iic_master_multi_100us(void); 

	extern __attribute__(( weak )) void init_bme280(void); 
	extern __attribute__(( weak )) void deinit_bme280(void); 
	extern __attribute__(( weak )) void do_bme280(void); 
	extern __attribute__(( weak )) void isr_bme280_1ms(void); 

	extern __attribute__(( weak )) void init_dht11(void); 
	extern __attribute__(( weak )) void do_dht11(void); 
	extern __attribute__(( weak )) void deinit_dht11(void); 
	extern __attribute__(( weak )) void isr_dht11_1ms(void); 
	extern __attribute__(( weak )) void isr_dht11_external_interrupt(void); 

	extern __attribute__(( weak )) void init_ccs811(void); 
	extern __attribute__(( weak )) void do_ccs811(void); 
	extern __attribute__(( weak )) void deinit_ccs811(void); 
	extern __attribute__(( weak )) void isr_ccs811_1ms(void); 

	extern __attribute__(( weak )) unsigned char getBME280Temp_update(void) { return 0;}
	extern __attribute__(( weak )) float getBME280Temp(void) { return 0.0; }
	extern __attribute__(( weak )) float getBME280Pressure(void) { return 0.0; }
	extern __attribute__(( weak )) float getBME280Altitude(void) { return 0.0; }
	extern __attribute__(( weak )) float getBME280Humidity(void) { return 0.0; }
	extern __attribute__(( weak )) float getBME280PressureAtSeeLevel(float altitude) { return 0.0; }

	extern __attribute__(( weak )) void init_app_thingspeak_wifi(void);
	extern __attribute__(( weak )) void do_app_thingspeak_wifi(void);
	extern __attribute__(( weak )) void isr_app_thingspeak_wifi_1ms(void);

	extern __attribute__(( weak )) void init_app_thingspeak_lora(void);
	extern __attribute__(( weak )) void do_app_thingspeak_lora(void);
	extern __attribute__(( weak )) void isr_app_thingspeak_lora_1ms(void);

	extern __attribute__(( weak )) void init_wifi(void); 
	extern __attribute__(( weak )) void do_wifi(void); 
	extern __attribute__(( weak )) void isr_eint_wifi(void); 
	extern __attribute__(( weak )) void isr_wifi_1ms(void); 

	extern __attribute__(( weak )) void init_serial_usb(void);
	extern __attribute__(( weak )) void init_usb(void);
	extern __attribute__(( weak )) void do_usb(void);
	extern __attribute__(( weak )) void do_serial_usb(void);
	extern __attribute__(( weak )) void isr_usb_1ms(void);

	extern __attribute__(( weak )) void init_spi_sw(void);
	extern __attribute__(( weak )) void deinit_spi_sw(void);
	extern __attribute__(( weak )) void do_spi_sw(void);
	extern __attribute__(( weak )) void isr_spi_sw_1ms(void);

	extern __attribute__(( weak )) void init_sx1276(void);
	extern __attribute__(( weak )) void do_sx1276(void);
	extern __attribute__(( weak )) void isr_sx1276_100us(void);

	extern __attribute__(( weak )) void init_wakeup(void);
	extern __attribute__(( weak )) void do_wakeup(void);
	extern __attribute__(( weak )) void isr_sx1276_1ms(void);
	
#endif
