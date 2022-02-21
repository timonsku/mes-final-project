#include <atmel_start.h>
#include <stdio.h>

int main(void)
{
	//set flash wait state. 1 is needed for 48Mhz @ 3.3V operation.
	//See Datasheet Table 37-42 page 891
	NVMCTRL->CTRLB.bit.RWS = 1;
	// Initializes MCU, drivers and middleware
	atmel_start_init();

	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_0, &io);
	usart_sync_enable(&USART_0);

	uint8_t adc_result_message[4];
	// start character
	adc_result_message[0] = 127;
	// stop character
	adc_result_message[3] = 255;


	// following Adafruit Metro M0 pinout/labels
	uint8_t muxposADC[] = {ADC_INPUTCTRL_MUXPOS_PIN0,ADC_INPUTCTRL_MUXPOS_PIN2,ADC_INPUTCTRL_MUXPOS_PIN3,ADC_INPUTCTRL_MUXPOS_PIN4};
	//default channel
	uint8_t currentADC = selectADC[0];

	adc_sync_enable_channel(&ADC_0, 0);
	adc_sync_enable_channel(&ADC_0, 1);
	adc_sync_enable_channel(&ADC_0, 2);
	adc_sync_enable_channel(&ADC_0, 3);

	ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1;

	ADC->REFCTRL.reg = ADC_REFCTRL_REFSEL_INTVCC1;
	ADC->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT;

	ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2 |
	ADC_INPUTCTRL_MUXNEG_GND | ADC_INPUTCTRL_MUXPOS_PIN0;

	char strDbg[100];
	while (1) {
		if(usart_sync_is_rx_not_empty(&USART_0)){
			uint8_t buf[3];
			io_read(io,buf,1);
			if(buf[0] == 127){
				io_read(io,buf,2);
				if (buf[1] == 255){
					sprintf(strDbg, "Selecting ADC chan %d\n\r", selectADC[buf[0]]);
					cdcdf_acm_write(strDbg, 100);
					ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2 |
										ADC_INPUTCTRL_MUXNEG_GND |
										muxposADC[buf[0]];
				}
			}
		}
		uint8_t adcResult[2] = {0};
		adc_sync_read_channel(&ADC_0, currentADC, adcResult, 2);
		// lets look at the returned bytes separately to spot issues more easily
		sprintf(strDbg, "ADC result %d,%d\n\r", adcResult[0], adcResult[1]);
		cdcdf_acm_write(strDbg, 100);
		adc_result_message[1] = adcResult[0];
		adc_result_message[2] = adcResult[1];
		io_write(io, adc_result_message, 4);
		delay_ms(50);
	}
}
