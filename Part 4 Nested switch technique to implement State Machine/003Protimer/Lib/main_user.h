#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>

// PORT A
#define PIN_BUTTON1 GPIO_PIN_1 // INC_TIME
#define PIN_BUTTON2 GPIO_PIN_2 // DEC_TIME
#define PIN_BUTTON3 GPIO_PIN_3 // START/PAUSE
#define PIN_BUZZER  GPIO_PIN_4 // BUZZER

// LCD CONNECTIONS - PORT B
#define PIN_LCD_SCL 6
#define PIN_LCD_SDA 7

// button pad truth table
#define BTN_PAD_VALUE_INC_TIME 4
#define BTN_PAD_VALUE_DEC_TIME 2
#define BTN_PAD_VALUE_ABRT     6
#define BTN_PAD_VALUE_SP       1

// state of button
typedef enum{
	NOT_PRESSED,
	BOUNCE,
	PRESSED
}button_state_t;

/* Signals of the application */
typedef enum{
	INC_TIME,
	DEC_TIME,
	TIME_TICK,
	START_PAUSE,
	ABRT,
/* Internal activity signals */
	ENTRY,
	EXIT
}protimer_signal_t;

/* Various States of the application */
typedef enum{
	IDLE,
	TIME_SET,
	COUNTDOWN,
	PAUSE,
	STAT
}protimer_state_t;
 
/* Main application structure */
typedef struct{
	uint32_t curr_time;
	uint32_t elapsed_time;
	uint32_t pro_time;
	protimer_state_t active_state;
}protimer_t;

/* Genertic(Super) event structure */
typedef struct{
	uint8_t sig;
}event_t;

/* For user generated events */
typedef struct{
	event_t super;
}protimer_user_event_t;

/* For tick event */
typedef struct{
	event_t super;
	uint8_t ss;
}protimer_tick_event_t;

typedef enum{
	EVENT_HANDLED,
	EVENT_IGNORED,
	EVENT_TRANSITION
}event_status_t;

void protimer_init(protimer_t *mobj);
event_status_t protimer_state_machine(protimer_t *const mobj, event_t const *const e);
#endif