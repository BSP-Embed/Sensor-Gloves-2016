#include "main.h"

extern int8u lcdptr;
static volatile int8u mode = CTRL;
 
const char *MSG[] = { 
					"I'm in Emergency condition. Please Help Me.",
					};
					
int8u SYM[][5] = {
					33,44,43,23,28,		/* H */
					34,25,29,27,31,		/* E */
					46,44,27,24,29,		/* L */
					37,27,33,30,36,		/* O */
				};

int main(void)
{
	int8u Sym;
	init();
	
	while (TRUE) {
		
		if (Flag.Sym) {
			Flag.Sym = FALSE;
			Sym = CheckSym();
			if (Sym) {
				lcdclrr(1);
				lcdws("Checking Symbol");
				beep(1,ALRT_DLY);
				dlyms(DISP_DLY);
				switch (mode) {
					case VOICE: lcdclrr(1);
					lcdws("Voice Control:");
					switch(Sym) {
						case 1: lcdwd('H'); break;
						case 2: lcdwd('E'); break;
						case 3: lcdwd('L'); break;
						case 4: lcdwd('O'); break;
					}
					EnVoice(Sym);
					break;
					case CTRL:	lcdclrr(1);
						lcdws("Device Control");
						dlyms(DISP_DLY);
						Trans(Sym);
						break;
				}
				disptitl();
			}
			Flag.Sym = TRUE;
			Flag.Mchg = TRUE;
		}
		
		if (Flag.EmrSw) {
			Flag.EmrSw = FALSE;
			lcdclrr(1);
			lcdws("Emergency Cond'n");
			beep(1,ALRT_DLY);
			dlyms(DISP_DLY);
			GSMsndmsg(RPhNum, MSG[0]);
			disptitl();
			Flag.Mchg = TRUE;
		}
		
		if (Flag.Mchg) {
			Flag.Mchg = FALSE;		
			lcdptr = 0xc5;
			if (mode == VOICE)
				lcdws("VOICE CTRL ");
			else
				lcdws("DEVICE CTRL");
			//beep(1,IND_DLY);
		}
			
		sleep();
		
	}
	return 0;
}
static void init(void)
{
	buzinit();
	beep(2,100);
	gpioinit();
	ledinit();
	lcdinit();
	uartinit();
	tmr1init();
	adcinit();
	EXT0init();
	GSMinit();
	disptitl();
	sei();
		
}
static void EnVoice(int8u ch)
{
	APR_PORT &= ~_BV(ch+3);
	dlyms(100);
	APR_PORT |= _BV(ch+3);
	dlyms(2500);
}

static void gpioinit(void)
{
	MODE_SW_DDR &= ~_BV(MODE_SW_PIN);
	MODE_SW_PORT |= _BV(MODE_SW_PIN);
	
	RF_DDR |= 0xF8;
	
	RF_PORT &= 0x0F;
	RF_PORT |= _BV(RF_EN_PIN);
	
	APR_DDR	  |= 0xF0;
	APR_PORT |= 0xF0;
}

static void disptitl(void)
{ 
	lcdclr();
	lcdws("  Sensor Glove");
	lcdr2();
	lcdws("Mode:");
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
	
	if (++j >= 2) {
		j = 0;
		Flag.Sym = TRUE;
	}
	
	if (++k >= 10) {
		k = 0;
		if ((MODE_SW_PPIN & _BV(MODE_SW_PIN)) == 0)	{
			if (Flag.chg == FALSE) {
				mode = VOICE;
				Flag.chg = TRUE;
				Flag.Mchg = TRUE;
			}
		}
		else {
			if (Flag.chg == TRUE) {
				mode = CTRL;
				Flag.chg = FALSE;
				Flag.Mchg = TRUE;
			}
		}
	}
	
}
ISR(INT0_vect) 
{ 
	Flag.EmrSw = TRUE;
	GICR |= _BV(INT0);
}
ISR(INT1_vect)
{
//	Flag.ChkSymSw = TRUE;
	GICR |= _BV(INT0);
}
static int8u CheckSym(void)
{
	int8u i,j;
	char TempStr[10];
	int8u FlexVal[5];
	
	for (i = 0; i < 5; i++) {
		FlexVal[i] = adcget(i) / 20;
		itoa(FlexVal[i], TempStr);
		lcdptr = 0x80 + (i * 3);
		lcdws("   ");
		lcdptr = 0x80 + (i * 3);
		lcdws(TempStr);
	}
	
	for (j = 0; j < 4; j++) {
		Flag.Sym = TRUE;
		for (i = 0; i < 5; i++) {
			if ((FlexVal[i] < (SYM[j][i] + DELTA)) && (FlexVal[i] > (SYM[j][i] - DELTA)))
				;
			else {
				Flag.Sym = FALSE;
				break;
			}
		}
		if (Flag.Sym) {
			return j+1;
			break;
		} 
	}
	return 0;
}

static void Trans(int8u symb)
{
	RF_PORT = (RF_PORT & 0x0F) | (symb << 4);
	RF_PORT &= ~_BV(RF_EN_PIN);
	dlyms(1000);
	RF_PORT |= _BV(RF_EN_PIN);	
}





