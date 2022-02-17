
#include <asf.h>

#define LED0    IOPORT_CREATE_PIN(IOPORT_PORTA, 17)

int main (void)
{
	system_init();
	while (1)
	{
		ioport_toggle_pin_level(LED0);
		delay_ms(1000);
	}
	/* Insert application code here, after the board has been initialized. */
}
