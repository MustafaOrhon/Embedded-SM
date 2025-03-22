#include "main.hpp"

static eEvent_Status ProtimerStateHandler_Idle(sProtimer_t * const mobj,sEvent_t const * const e);
static eEvent_Status ProtimerStateHandler_Time_Set(sProtimer_t * const mobj,sEvent_t const * const e);
static eEvent_Status ProtimerStateHandler_Pause(sProtimer_t * const mobj,sEvent_t const * const e);
static eEvent_Status ProtimerStateHandler_Countdown(sProtimer_t * const mobj,sEvent_t const * const e);
static eEvent_Status ProtimerStateHandler_Stat(sProtimer_t * const mobj,sEvent_t const * const e);
static void display_time(uint32_t time);
static void display_message(String s,uint8_t c , uint8_t r);
static void display_clear(void);
static void do_beep(void);

#define IDLE        &ProtimerStateHandler_Idle
#define TIMESET     &ProtimerStateHandler_Time_Set
#define PAUSE       &ProtimerStateHandler_Pause
#define COUNTDOWN   &ProtimerStateHandler_Countdown
#define STAT        &ProtimerStateHandler_Stat


void Protimer_Init(sProtimer_t * const mobj)
{
    sEvent_t ee;
    ee.signal = eProtimerSignal_Entry;
    mobj->active_state = IDLE;
    mobj->proactive_time = 0;
    (*mobj->active_state)(mobj,&ee); // Jump to Idle Handler
}

static eEvent_Status ProtimerStateHandler_Idle(sProtimer_t * const mobj, sEvent_t const * const e)
{
    switch (e->signal)
    {

    case eProtimerSignal_Entry:
    {
        mobj->current_time = 0;
        mobj->elapsed_time = 0;
        display_time(0);
        display_message("SET",0,0);
        display_message("TIME",0,1);
        return eEvent_Handled;
    }
    case eProtimerSignal_Exit:
    {
        display_clear();
        return eEvent_Handled;
    }
    case eProtimerSignal_IncTime:
    {
        mobj->current_time += 60;
        mobj->active_state = TIMESET;
        return eEvent_Transition;
    }
    case eProtimerSignal_StartPause:
    {
        mobj->active_state = STAT;
        return eEvent_Transition;
    }
    case eProtimerSignal_TickTime:
    {
        if(((sProTimerTickEvent_t *)(e))->ss == 5)
        {
            do_beep();
            return eEvent_Handled;
        }
        return eEvent_Ignored;
    }
    }
    return eEvent_Ignored;
}

static eEvent_Status ProtimerStateHandler_Time_Set(sProtimer_t * const mobj,sEvent_t const * const e)
{
    switch (e->signal)
    {

    case eProtimerSignal_Entry:
    {
        display_time(mobj->current_time);
        return eEvent_Handled;
    }
    case eProtimerSignal_Exit:
    {
        display_clear();
        return eEvent_Handled;
    }
    case eProtimerSignal_IncTime:
    {
        mobj->current_time += 60;
        display_time(mobj->current_time);
        return eEvent_Handled;
    }
    case eProtimerSignal_DecTime:
    {
        if(mobj->current_time >= 60)
        {
            mobj->current_time -= 60;
            display_time(mobj->current_time);
            return eEvent_Handled;
        }
        return eEvent_Ignored;
    }
    case eProtimerSignal_StartPause:
    {
        if(mobj->current_time >= 60)
        {
            mobj->active_state = COUNTDOWN;
            return eEvent_Transition;
        }
        return eEvent_Ignored;
    }
    case eProtimerSignal_TickTime:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_ABRT:
    {
        mobj->active_state = IDLE;
        return eEvent_Transition;
    }
    }
    return eEvent_Ignored;
}

static eEvent_Status ProtimerStateHandler_Pause(sProtimer_t * const mobj,sEvent_t const * const e)
{
    switch (e->signal)
    {

    case eProtimerSignal_Entry:
    {
        display_message("Paused",5,1);
        return eEvent_Handled;
    }
    case eProtimerSignal_Exit:
    {
        display_clear();
        return eEvent_Handled;
    }
    case eProtimerSignal_IncTime:
    {
        mobj->current_time += 60;
        mobj->active_state = TIMESET;
        return eEvent_Transition;
    }
    case eProtimerSignal_DecTime:
    {
        if(mobj->current_time >= 60)
        {
            mobj->current_time -= 60;
            mobj->active_state = TIMESET;
            return eEvent_Transition;
        }
        return eEvent_Ignored;
    }
    case eProtimerSignal_StartPause:
    {
        mobj->active_state = COUNTDOWN;
        return eEvent_Transition;

    }
    case eProtimerSignal_TickTime:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_ABRT:
    {
        mobj->active_state = IDLE;
        return eEvent_Transition;
    }
    }
    return eEvent_Ignored;
}

static eEvent_Status ProtimerStateHandler_Countdown(sProtimer_t * const mobj,sEvent_t const * const e)
{
    switch (e->signal)
    {

    case eProtimerSignal_Entry:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_Exit:
    {
        mobj->proactive_time += mobj->elapsed_time;
        mobj->elapsed_time = 0;
        return eEvent_Handled;
    }
    case eProtimerSignal_IncTime:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_DecTime:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_StartPause:
    {
        mobj->active_state = PAUSE;
        return eEvent_Transition;
    }
    case eProtimerSignal_TickTime:
    {
        if(((sProTimerTickEvent_t *)(e))->ss == 10)
        {
            --mobj->current_time;
            ++mobj->elapsed_time;
            display_time(mobj->current_time);
            if(mobj->current_time <= 0)
            {
                mobj->active_state = IDLE;
                return eEvent_Transition;
            }
            else
            {
                return eEvent_Handled;
            }
        }
        return eEvent_Ignored;
    }
    case eProtimerSignal_ABRT:
    {
        mobj->active_state = IDLE;
        return eEvent_Transition;
    }
    }
    return eEvent_Ignored;
}

static eEvent_Status ProtimerStateHandler_Stat(sProtimer_t * const mobj,sEvent_t const * const e)
{
    static uint8_t tick_count = 0;
    switch (e->signal)
    {

    case eProtimerSignal_Entry:
    {
        display_time(mobj->proactive_time);
        display_message("Productive Time",1,1);
        return eEvent_Handled;
    }
    case eProtimerSignal_Exit:
    {
        display_clear();
        return eEvent_Handled;
    }
    case eProtimerSignal_IncTime:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_DecTime:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_StartPause:
    {
        return eEvent_Ignored;
    }
    case eProtimerSignal_TickTime:
    {
        if(++tick_count == 40)
        {
            tick_count = 0;
            mobj->active_state = IDLE;
            return eEvent_Transition;
        }
        return eEvent_Ignored;
    }
    case eProtimerSignal_ABRT:
    {
        return eEvent_Ignored;
    }
    }
    return eEvent_Ignored;
}

/* HELPER FUNCTIONS */
static void display_time(uint32_t time) {
    String time_msg;
  
    uint16_t m = time / 60;
    uint8_t s = time % 60;
     
    time_msg = String(m);
    time_msg.concat(':');
    if (s < 10) {
        time_msg.concat('0');  
    }
    time_msg.concat(String(s));
    
    lcd_set_cursor(5, 0);
    lcd_print_string(time_msg);
}
  
static void display_message(String s,uint8_t c , uint8_t r)
{
    lcd_set_cursor(c,r);
    lcd_print_string(s);
  }
  
  static void display_clear(void){
    lcd_clear();
  }
  
  static void do_beep(void){
    tone(PIN_BUZZER, 4000, 25);
  }