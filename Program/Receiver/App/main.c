#include "main.h"

extern int8u lcdptr;

int main(void)
{
	init();
	
	while (TRUE)
		sleep();
	
	return 0;
}
static void init(void)
{
	buzinit();
	beep(2,100);
	gpioinit();
	ledinit();
	lcdinit();
	tmr1init();
	EXT0init();
	disptitl();
	sei();
}

static void gpioinit(void)
{
	RF_DDR &= 0x0F;
	RF_PORT |= 0xF0;
	
	DEVICE_DDR |= _BV(LIGHT_PIN) | _BV(FAN_PIN);
	DEVICE_PORT &= ~(_BV(LIGHT_PIN) | _BV(FAN_PIN));

}

static void disptitl(void)
{ 
	lcdclr();
	lcdws("  Sensor Glove");
	lcdr2();
	lcdws(" Device Control");
}
static void tmr1init(void)
{
	TCNT1H   = 0xD3;
	TCNT1L   = 0x00;
	TIMSK   |= _BV(TOIE1);			//ENABLE OVERFLOW INTERRUPT
	TCCR1A   = 0x00;					
	TCCR1B  |= _BV(CS10) | _BV(CS11); /* PRESCALAR BY 16 */
}
static void EXT0init(void)
{
	INTDDR 	&= ~_BV(INT0_PIN);
	INTPORT |= _BV(INT0_PIN);

	INTDDR 	&= ~_BV(INT1_PIN);
	INTPORT |= _BV(INT1_PIN);

	GICR |= _BV(INT0) | _BV(INT1);			//ENABLE EXTERNAL INTERRUPT
	MCUCR |= _BV(ISC01) | _BV(ISC11);		//FALLING EDGE INTERRUPT

}

/* overflows at every 100msec */
ISR(TIMER1_OVF_vect) 
{ 
	static int8u i,j,k;

	TCNT1H = 0xD3;
	TCNT1L = 0x00;
	
	if (++i >= 50) i = 0;
	switch(i) {
		case 0: case 2: ledon();  break;
		case 1: case 3: ledoff(); break;
	} 
}
ISR(INT1_vect) 
{ 
	lcdclrr(1);
	switch((RF_PPIN & 0xF0) >> 4) {
		case LIGHT_ON: LitOn(); lcdws ("    Light ON"); break;
		case LIGHT_OFF: LitOff(); lcdws ("   Light OFF"); break;
		case FAN_ON: FanOn(); lcdws ("     Fan ON"); break;
		case FAN_OFF: FanOff(); lcdws ("    Fan OFF"); break;
		default: lcdws ("  INVALID CODE"); break;
	}
	beep(1,ALRT_DLY);
	dlyms(DISP_DLY);
	
	disptitl();
	
	GICR |= _BV(INT1);
}







