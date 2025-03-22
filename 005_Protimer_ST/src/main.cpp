#include "main.hpp"

static void protimer_event_dispatcher(sProtimer_t * const mobj,sEvent_t const * const e);
static uint8_t process_button_pad_value(uint8_t btn_pad_value);
static void display_init(void);
static void protimer_state_table_init(sProtimer_t * const mobj);
static sProtimer_t protimer{0};

void setup() {
  Serial.begin(9600);
  display_init();
  Serial.println("Productive timer application.");
  Serial.println("=============================");
  pinMode(PIN_BUTTON1,INPUT);
  pinMode(PIN_BUTTON2,INPUT);
  pinMode(PIN_BUTTON3,INPUT);
  protimer_state_table_init(&protimer);
  Protimer_Init(&protimer);

}

void loop() {
  static uint32_t current_time = millis();
  static sProTimerTickEvent_t te;
  //Read the button pad status
  uint8_t b1,b2,b3,button_pad_value;
  sProtimerUserEvent_t ue;
  b1 = digitalRead(PIN_BUTTON1);
  b2 = digitalRead(PIN_BUTTON2);
  b3 = digitalRead(PIN_BUTTON3);
  button_pad_value = (b1 << 2) | (b2 << 1) | (b3);

  button_pad_value = process_button_pad_value(button_pad_value);
  // Make an event
  if(button_pad_value)
  {
    if(button_pad_value == BTN_PAD_VALUE_INC_TIME)
    {
      Serial.println("Enter the Increment.");
      ue.super.signal= eProtimerSignal_IncTime;
    }
    else if(button_pad_value == BTN_PAD_VALUE_DEC_TIME)
    {
      ue.super.signal= eProtimerSignal_DecTime;
    }
    else if(button_pad_value == BTN_PAD_VALUE_INC_ABRT)
    {
      ue.super.signal= eProtimerSignal_ABRT;
    }
    else if(button_pad_value == BTN_PAD_VALUE_INC_SP)
    {
      Serial.println("Entere the start pause.");
      ue.super.signal= eProtimerSignal_StartPause;
    }
    // Send it to event dispatcher
    protimer_event_dispatcher(&protimer,&ue.super);
  }

  //4. Dispatch the timetick event for every 100ms
  if(millis() - current_time >= 100)
  {
    current_time = millis();
    te.super.signal = eProtimerSignal_TickTime;
    if(++te.ss > 10)
    {
      te.ss = 1;
    }
    protimer_event_dispatcher(&protimer,&te.super);
  }
}

static void protimer_event_dispatcher(sProtimer_t * const mobj,sEvent_t const * const e)
{
  eEvent_Status status;
  eProtimerState_t source, target;
  e_handler_t ehandler;
  source = mobj->active_state;

  ehandler = (e_handler_t)mobj->state_table[mobj->active_state * eProtimerState_Last + e->signal];
  if(ehandler)
  {
    status = (*ehandler)(mobj,e);
  }
  if(status == eEvent_Transition)
  {
    target = mobj->active_state;
    sEvent_t ev;
    //1. Run the exit action for the source state
    ev.signal = eProtimerSignal_Exit;
    ehandler = (e_handler_t)mobj->state_table[source* eProtimerState_Last + eProtimerSignal_Exit];
    if(ehandler)
    {
      (*ehandler)(mobj,&ev);
    }
    //2. Run the Entry action for the target state
    ev.signal = eProtimerSignal_Entry;
    ehandler = (e_handler_t)mobj->state_table[target* eProtimerState_Last + eProtimerSignal_Entry];
    if(ehandler)
    {
      (*ehandler)(mobj,&ev);
    }
  }
}

static void protimer_state_table_init(sProtimer_t * const mobj) {
  static e_handler_t protimer_state_table[eProtimerState_Last][eProtimerSignal_Last] = 
  {
    [eProtimerState_Idle] = {&IDLE_INC_TIME, NULL, &IDLE_TICK_TIME, &IDLE_START_PAUSE, NULL, &IDLE_ENTRY, &IDLE_EXIT},
    [eProtimerState_Time_Set] = {&TIME_SET_INC_TIME, &TIME_SET_DEC_TIME, NULL, &TIME_SET_START_PAUSE, &TIME_SET_ABRT, &TIME_SET_ENTRY, &TIME_SET_EXIT},
    [eProtimerState_Countdown] = {NULL, NULL, &COUNTDOWN_TICK_TIME, &COUNTDOWN_START_PAUSE, &COUNTDOWN_ABRT, NULL, &COUNTDOWN_EXIT},
    [eProtimerState_Pause] = {&PAUSE_INC_TIME, &PAUSE_DEC_TIME, NULL, &PAUSE_START_PAUSE, &PAUSE_ABRT, &PAUSE_ENTRY, &PAUSE_EXIT},
    [eProtimerState_Stat] = {NULL, NULL, &STAT_TICK_TIME, NULL, NULL, &STAT_ENTRY, &STAT_EXIT},
  };

  mobj->state_table = (uintptr_t * )&protimer_state_table[0][0];
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