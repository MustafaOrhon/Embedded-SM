#include "main.hpp"
#include "qpn.h"
#include "ClockAlarm_SM.h"
Q_DEFINE_THIS_FILE;

static void TIMER1_Config(void);
static void display_init(void);
static uint8_t process_button_pad_value(uint8_t btn_pad_value);
void setup() 
{
    Serial.begin(9600);
    Serial.println("Timer Test.!");
    display_init();
    pinMode(PIN_BUTTON1,INPUT);
    pinMode(PIN_BUTTON2,INPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, HIGH);
    Clock_Alarm_ctor();
    QHSM_INIT(super_ClockAlarm);
    TIMER1_Config();
    Serial.print('\n');
}

void loop() 
{
    static uint32_t tick_time = millis();
    static uint32_t alarm_check_time = millis();
    uint8_t b1,b2,button_pad_value;
    b1 = digitalRead(PIN_BUTTON1);
    b2 = digitalRead(PIN_BUTTON2);

    button_pad_value = (b2 << 1) | (b1);
    button_pad_value = process_button_pad_value(button_pad_value);
    if(button_pad_value)
    {
        if(button_pad_value == BTN_PAD_VALUE_SET)
        {
            Serial.println("Enter the Set.");
            Q_SIG(super_ClockAlarm) = (QSignal)SET_SIG;
            QHSM_DISPATCH(super_ClockAlarm);
        }
        else if(button_pad_value == BTN_PAD_VALUE_OK)
        {
            Serial.println("Enter the OK.");
            Q_SIG(super_ClockAlarm) = (QSignal)OK_SIG;
            QHSM_DISPATCH(super_ClockAlarm);
        }
        else if(button_pad_value == BTN_PAD_VALUE_ABRT)
        {
            Serial.println("Enter the ABRT.");
            Q_SIG(super_ClockAlarm) = (QSignal)ABRT_SIG;
            QHSM_DISPATCH(super_ClockAlarm);
        }
    }
    while(millis() - tick_time >= 50)
    {
        tick_time = millis();
        Q_SIG(super_ClockAlarm) = (QSignal)TICK_SIG;
        QHSM_DISPATCH(super_ClockAlarm);
    }

    while(millis() - alarm_check_time >= 500)
    {
        alarm_check_time = millis();
        Q_SIG(super_ClockAlarm) = (QSignal)ALARM_SIG;
        QHSM_DISPATCH(super_ClockAlarm);
    }
}

Q_NORETURN Q_onAssert (char_t const Q_ROM *const module, int_t const location)
{
  Serial.println("Assertion failed!!");
  Serial.println((String)module);
  Serial.println(location);
  while(1);
}

static void TIMER1_Config(void)
{
    TCCR1A = 0; // CTC Mode
    TCCR1B = B00001100; //Prescaler to 256
    TIMSK1 |= B00000010; // Interrupt enable for OCR1A compare match
    OCR1A = 6250 - 1; //OC Match value for 100ms;
}

static uint8_t process_button_pad_value(uint8_t btn_pad_value) {
    static const uint16_t DEBOUNCE_TIME_MS = 50;
    static const uint16_t HOLD_THRESHOLD_MS   = 800;
    static const uint16_t REPEAT_INTERVAL_MS  = 200;
  
    static struct {
        uint8_t state;
        uint32_t debounce_time;
        uint32_t hold_start_time;
        uint32_t last_repeat_time;
        uint8_t last_button;
        bool is_held;
    } btn_ctx = { eButtonState_NotPressed, 0, 0, 0, 0, false };
  
    uint32_t current_time = millis();
  
    // State: Not Pressed
    if (btn_ctx.state == eButtonState_NotPressed) {
        if (btn_pad_value > 0) {
            btn_ctx.state = eButtonState_Bounce;
            btn_ctx.debounce_time = current_time;
        }
        return 0;
    }
  
    // State: Bounce
    if (btn_ctx.state == eButtonState_Bounce) {
        if (current_time - btn_ctx.debounce_time < DEBOUNCE_TIME_MS)
            return 0;  // Still bouncing
  
        // Bounce period finished; check if button is still pressed
        if (btn_pad_value > 0) {
            btn_ctx.state = eButtonState_Pressed;
            btn_ctx.hold_start_time = current_time;
            btn_ctx.last_button = btn_pad_value;
            return btn_pad_value;
        } else {
            btn_ctx.state = eButtonState_NotPressed;
            return 0;
        }
    }
  
    // State: Pressed
    if (btn_ctx.state == eButtonState_Pressed) {
        // Check for button release
        if (btn_pad_value <= 0) {
            btn_ctx.state = eButtonState_Bounce;
            btn_ctx.debounce_time = current_time;
            btn_ctx.is_held = false;
            return 0;
        }
  
        // Ensure we process events only for the same button
        if (btn_pad_value != btn_ctx.last_button)
            return 0;
  
        // Process hold event or repeated events
        if (btn_ctx.is_held == false) {
            // First hold event: trigger after threshold
            if (current_time - btn_ctx.hold_start_time >= HOLD_THRESHOLD_MS) {
                btn_ctx.is_held = true;
                btn_ctx.last_repeat_time = current_time;
                return btn_pad_value;
            }
            return 0;
        }
        if (current_time - btn_ctx.last_repeat_time >= REPEAT_INTERVAL_MS) {
            btn_ctx.last_repeat_time = current_time;
            return btn_pad_value;
        }
        return 0;
    }
  
    return 0;
  }

static void display_init(void)
{
  lcd_init();
  lcd_clear();
  lcd_move_cursor_L_to_R();
  lcd_set_cursor(0,0);
  lcd_no_auto_scroll();
  lcd_cursor_off();
}