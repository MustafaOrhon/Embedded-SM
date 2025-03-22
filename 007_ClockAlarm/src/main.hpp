#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "lcd.hpp"

#define PIN_BUTTON1         2
#define PIN_BUTTON2         3
#define BTN_PAD_VALUE_SET   1
#define BTN_PAD_VALUE_OK    2
#define BTN_PAD_VALUE_ABRT  3

typedef enum
{
    eButtonState_NotPressed,
    eButtonState_Bounce,
    eButtonState_Pressed
}eButtonState_t;

#endif