#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "lcd.hpp"

#define PIN_BUTTON1         2
#define PIN_BUTTON2         3
#define PIN_BUZZER          9
#define BTN_PAD_VALUE_SET   1
#define BTN_PAD_VALUE_OK    2
#define BTN_PAD_VALUE_ABRT  3
#define MAX_QUEUE_EVENT_NUMBER 5

#define MCU_SYS_CLK_FREQ  16000000UL

#define TCCR1A_CTC_MODE                     (0U)
#define TCCR1B_CTC_MODE                     (1U << 3U)
#define TCCR1B_PRESCALER_1            0x1UL
#define TCCR1B_PRESCALER_8            0x2UL
#define TCCR1B_PRESCALER_64           0x3UL
#define TCCR1B_PRESCALER_256          0x4UL

#define TIMER1_PRESCALER_1               1UL
#define TIMER1_PRESCALER_8               8UL
#define TIMER1_PRESCALER_64              64UL
#define TIMER1_PRESCALER_256             256UL
#define TIMER1_PRESCALER_1024            1024UL

#define CONFIG_TCCR1B_PRESCALER       TCCR1B_PRESCALER_1
#define CONFIG_TIMER1_PRESCALER       TIMER1_PRESCALER_1
#define CONFIG_TICKS_PER_SECOND       1000UL

#define MS_PER_TICK                  (1000UL/CONFIG_TICKS_PER_SECOND)
#define TIMER1_OC_MATCH_VALUE               (((MCU_SYS_CLK_FREQ * MS_PER_TICK) / (CONFIG_TIMER1_PRESCALER * 1000UL))-1)

#define MS_TO_TICKS( timeInMs )               ( ( timeInMs) / (MS_PER_TICK) )

extern bool flag_report_button_press;

#endif