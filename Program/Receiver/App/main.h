#ifndef MAIN_H
#define MAIN_H

#include "includes.h"

#define INTDDR				DDRD
#define INTPORT				PORTD
#define INT0_PIN			PD2
#define INT1_PIN			PD3

#define DEVICE_DDR			DDRC
#define DEVICE_PORT			PORTC
#define LIGHT_PIN			PC2
#define FAN_PIN				PC3

#define RF_DDR				DDRD
#define RF_PORT				PORTD
#define RF_PPIN				PIND

//DEFINE CONSTANT
#define LitOn()				DEVICE_PORT |= _BV(LIGHT_PIN)
#define LitOff()			DEVICE_PORT &= ~_BV(LIGHT_PIN)

#define FanOn()				DEVICE_PORT |= _BV(FAN_PIN)
#define FanOff()			DEVICE_PORT &= ~_BV(FAN_PIN)

enum { LIGHT_ON = 1, LIGHT_OFF, FAN_ON, FAN_OFF};
	
//FUNCTION PROTOTYPES
static void		 init		(void);
static void 	disptitl 	(void);
static void 	tmr1init	(void);
static void		EXT0init	(void);	
static void		gpioinit	(void);

#endif
