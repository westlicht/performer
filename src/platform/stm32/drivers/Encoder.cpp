#include "Encoder.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define ENC_PORT GPIOC
#define ENC_SWITCH GPIO15
#define ENC_A GPIO13
#define ENC_B GPIO14
#define ENC_GPIO (ENC_SWITCH | ENC_A | ENC_B)

Encoder::Encoder(bool reverse) :
    _reverse(reverse)
{}

enum {
	IDLE,
	R1,
	R2,
	R3,
	L1,
	L2,
	L3,
};

void Encoder::init() {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(ENC_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, ENC_GPIO);
    _state = IDLE;
}

void Encoder::process() {
    // handle switch
    bool switchState = _switchDebouncer.debounce(!gpio_get(ENC_PORT, ENC_SWITCH));
    if (switchState != _switchState) {
        _switchState = switchState;
        _events.write(switchState ? Event::Down : Event::Up);
    }

    // handle encoder
    int b = gpio_get(ENC_PORT, ENC_A);
    int a = gpio_get(ENC_PORT, ENC_B);

    switch( _state ) {
    case IDLE:
	if( !a && b ) {
    	    _state = R1;
	} else if ( a && !b ) {
	    _state = L1;
	} 
	break;
    case R1:
    	if( !a && !b ) {
    	    _state = R2;
   	} else if ( a && b ) {
    	    _state = IDLE;
    	}
    	break;
    case R2:
    	if( a && !b ) {
    	    _state = R3;
    	} else if ( !a && b ) {
    	    _state = R1;
    	}
    	break;
    case R3:
    	if( a && b ) {
    	    _state = IDLE;
    	    _events.write(_reverse ? Event::Right : Event::Left);
    	} else if ( !a && !b ) {
    	    _state = R2;
    	}
    	break;
    case L1:
    	if( !a && !b ) {
    	    _state = L2;
    	} else if ( a && b ) {
    	    _state = IDLE;
    	} 
    	break;
    case L2:
	if( !a && b ) {
    	    _state = L3;
	} else if ( a && !b ) {
	    _state = L1;
	} 
	break;
    case L3:
	if( a && b ) {
    	    _state = IDLE;
	    _events.write(_reverse ? Event::Left : Event::Right);
	} else if ( !a && !b ) {
	    _state = L2;
	} 
	break;
    }
}
