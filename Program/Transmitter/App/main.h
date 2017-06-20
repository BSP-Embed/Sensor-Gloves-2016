#ifndef MAIN_H
#define MAIN_H

#include "includes.h"

#define INTDDR				DDRD
#define INTPORT				PORTD
#define INT0_PIN			PD2
#define INT1_PIN			PD3

#define MODE_SW_DDR			DDRB
#define MODE_SW_PORT		PORTB
#define MODE_SW_PPIN		PINB
#define MODE_SW_PIN			PB0

#define RF_DDR				DDRC
#define RF_PORT				PORTC
#define RF_EN_PIN			PC3

#define APR_DDR				DDRD
#define APR_PORT			PORTD

//DEFINE CONSTANT
#define	INFO_EE_ADDR		0x01

#define DELTA				5			

enum {VOICE = 1, CTRL};

//DEFINE MACROS
#define StartTmr()			TCCR0  	|= _BV(CS01)
#define StopTmr()			TCCR0  	&= ~_BV(CS01)


struct  {
	volatile int8u EmrSw:1;
	volatile int8u ChkSymSw:1;
	volatile int8u Sym:1;
	volatile int8u Mchg:1;
	volatile int8u chg:1;
} Flag;

//FUNCTION PROTOTYPES
static void		 init		(void);
static void 	disptitl 	(void);
static void 	tmr1init	(void);
static void		EXT0init	(void);	
static void		gpioinit	(void);
static int8u	CheckSym	(void);
static void		Trans		(int8u symb);
static void		EnVoice		(int8u ch);

#endif
