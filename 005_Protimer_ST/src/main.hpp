#pragma once
#include <Arduino.h>
#include "lcd.hpp"

#define PIN_BUTTON1    2
#define PIN_BUTTON2    3
#define PIN_BUTTON3    4
#define PIN_BUZZER     12

#define LCD_RS        5
#define LCD_RW        6
#define LCD_EN        7
#define LCD_4         8
#define LCD_5         9
#define LCD_6         10
#define LCD_7         11

#define BTN_PAD_VALUE_INC_TIME  4
#define BTN_PAD_VALUE_DEC_TIME  2
#define BTN_PAD_VALUE_INC_ABRT  6
#define BTN_PAD_VALUE_INC_SP    1

typedef enum
{
    eButtonState_NotPressed,
    eButtonState_Bounce,
    eButtonState_Pressed
}eButtonState_t;

typedef enum
{
    eProtimerSignal_First = 0,
    eProtimerSignal_IncTime = eProtimerSignal_First,
    eProtimerSignal_DecTime,
    eProtimerSignal_TickTime,
    eProtimerSignal_StartPause,
    eProtimerSignal_ABRT,
/* Internal Activity Signals */
    eProtimerSignal_Entry,
    eProtimerSignal_Exit,
    eProtimerSignal_Last
}eProtimerSignal_t;

typedef enum
{
    eProtimerState_First = 0,
    eProtimerState_Idle = eProtimerState_First,
    eProtimerState_Time_Set,
    eProtimerState_Countdown,
    eProtimerState_Pause,
    eProtimerState_Stat,
    eProtimerState_Last
}eProtimerState_t;

typedef struct
{
    uint32_t current_time;
    uint32_t elapsed_time;
    uint32_t proactive_time;
    eProtimerState_t active_state;
    uintptr_t *state_table;
}sProtimer_t;

typedef struct
{
    uint8_t signal;
}sEvent_t;

typedef struct
{
    sEvent_t super;
}sProtimerUserEvent_t;

typedef struct
{
    sEvent_t super;
    uint8_t ss;
}sProTimerTickEvent_t;

typedef enum
{
    eEvent_Handled,
    eEvent_Ignored,
    eEvent_Transition
}eEvent_Status;

typedef eEvent_Status(*e_handler_t)(sProtimer_t * const mobj,sEvent_t const * const e);

void Protimer_Init(sProtimer_t *mobj);
eEvent_Status Protimer_StateMachine(sProtimer_t * const mobj,sEvent_t const * const e);

// Idle State Function Prototypes
eEvent_Status IDLE_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status IDLE_EXIT(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status IDLE_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status IDLE_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status IDLE_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e);

// Time_Set State Function Prototypes
eEvent_Status TIME_SET_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status TIME_SET_EXIT(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status TIME_SET_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status TIME_SET_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status TIME_SET_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status TIME_SET_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status TIME_SET_ABRT(sProtimer_t * const mobj, sEvent_t const * const e);

// Pause State Function Prototypes
eEvent_Status PAUSE_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status PAUSE_EXIT(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status PAUSE_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status PAUSE_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status PAUSE_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status PAUSE_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status PAUSE_ABRT(sProtimer_t * const mobj, sEvent_t const * const e);

// Countdown State Function Prototypes
eEvent_Status COUNTDOWN_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status COUNTDOWN_EXIT(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status COUNTDOWN_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status COUNTDOWN_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status COUNTDOWN_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status COUNTDOWN_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status COUNTDOWN_ABRT(sProtimer_t * const mobj, sEvent_t const * const e);

// Stat State Function Prototypes
eEvent_Status STAT_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status STAT_EXIT(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status STAT_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status STAT_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status STAT_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status STAT_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e);
eEvent_Status STAT_ABRT(sProtimer_t * const mobj, sEvent_t const * const e);