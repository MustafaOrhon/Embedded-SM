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
    eProtimerState_Stat
}eProtimerState_t;

typedef struct
{
    uint32_t current_time;
    uint32_t elapsed_time;
    uint32_t proactive_time;
    eProtimerState_t active_state;
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

void Protimer_Init(sProtimer_t *mobj);
eEvent_Status Protimer_StateMachine(sProtimer_t * const mobj,sEvent_t const * const e);