#include "main.hpp"

static void display_time(uint32_t time);
static void display_message(String s,uint8_t c , uint8_t r);
static void display_clear(void);
static void do_beep(void);


void Protimer_Init(sProtimer_t * const mobj)
{
    e_handler_t ehandler;
    sEvent_t ee;
    ee.signal = eProtimerSignal_Entry;
    mobj->active_state = eProtimerState_Idle;
    mobj->proactive_time = 0;
    ehandler = (e_handler_t)mobj->state_table[eProtimerState_Idle * eProtimerState_Last + eProtimerSignal_Entry];
    (*ehandler)(mobj,&ee);
}

// Idle State Functions
eEvent_Status IDLE_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->current_time = 0;
    mobj->elapsed_time = 0;
    display_time(0);
    display_message("SET",0,0);
    display_message("TIME",0,1);
    return eEvent_Handled;
}

eEvent_Status IDLE_EXIT(sProtimer_t * const mobj, sEvent_t const * const e) {
    display_clear();
    return eEvent_Handled;
}

eEvent_Status IDLE_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->current_time += 60;
    mobj->active_state = eProtimerState_Time_Set;
    return eEvent_Transition;
}

eEvent_Status IDLE_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->active_state = eProtimerState_Stat;
    return eEvent_Transition;
}

eEvent_Status IDLE_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    if(((sProTimerTickEvent_t *)(e))->ss == 5) {
        do_beep();
        return eEvent_Handled;
    }
    return eEvent_Ignored;
}

// Time_Set State Functions
eEvent_Status TIME_SET_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e) {
    display_time(mobj->current_time);
    return eEvent_Handled;
}

eEvent_Status TIME_SET_EXIT(sProtimer_t * const mobj, sEvent_t const * const e) {
    display_clear();
    return eEvent_Handled;
}

eEvent_Status TIME_SET_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->current_time += 60;
    display_time(mobj->current_time);
    return eEvent_Handled;
}

eEvent_Status TIME_SET_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    if(mobj->current_time >= 60) {
        mobj->current_time -= 60;
        display_time(mobj->current_time);
        return eEvent_Handled;
    }
    return eEvent_Ignored;
}

eEvent_Status TIME_SET_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e) {
    if(mobj->current_time >= 60) {
        mobj->active_state = eProtimerState_Countdown;
        return eEvent_Transition;
    }
    return eEvent_Ignored;
}

eEvent_Status TIME_SET_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status TIME_SET_ABRT(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->active_state = eProtimerState_Idle;
    return eEvent_Transition;
}

// Pause State Functions
eEvent_Status PAUSE_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e) {
    display_message("Paused",5,1);
    return eEvent_Handled;
}

eEvent_Status PAUSE_EXIT(sProtimer_t * const mobj, sEvent_t const * const e) {
    display_clear();
    return eEvent_Handled;
}

eEvent_Status PAUSE_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->current_time += 60;
    mobj->active_state = eProtimerState_Time_Set;
    return eEvent_Transition;
}

eEvent_Status PAUSE_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    if(mobj->current_time >= 60) {
        mobj->current_time -= 60;
        mobj->active_state = eProtimerState_Time_Set;
        return eEvent_Transition;
    }
    return eEvent_Ignored;
}

eEvent_Status PAUSE_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->active_state = eProtimerState_Countdown;
    return eEvent_Transition;
}

eEvent_Status PAUSE_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status PAUSE_ABRT(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->active_state = eProtimerState_Idle;
    return eEvent_Transition;
}

// Countdown State Functions
eEvent_Status COUNTDOWN_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status COUNTDOWN_EXIT(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->proactive_time += mobj->elapsed_time;
    mobj->elapsed_time = 0;
    return eEvent_Handled;
}

eEvent_Status COUNTDOWN_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status COUNTDOWN_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status COUNTDOWN_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->active_state = eProtimerState_Pause;
    return eEvent_Transition;
}

eEvent_Status COUNTDOWN_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    if(((sProTimerTickEvent_t *)(e))->ss == 10) {
        --mobj->current_time;
        ++mobj->elapsed_time;
        display_time(mobj->current_time);
        if(mobj->current_time <= 0) {
            mobj->active_state = eProtimerState_Idle;
            return eEvent_Transition;
        }
        else {
            return eEvent_Handled;
        }
    }
    return eEvent_Ignored;
}

eEvent_Status COUNTDOWN_ABRT(sProtimer_t * const mobj, sEvent_t const * const e) {
    mobj->active_state = eProtimerState_Idle;
    return eEvent_Transition;
}

// Stat State Functions
eEvent_Status STAT_ENTRY(sProtimer_t * const mobj, sEvent_t const * const e) {
    display_time(mobj->proactive_time);
    display_message("Productive Time",1,1);
    return eEvent_Handled;
}

eEvent_Status STAT_EXIT(sProtimer_t * const mobj, sEvent_t const * const e) {
    display_clear();
    return eEvent_Handled;
}

eEvent_Status STAT_INC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status STAT_DEC_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status STAT_START_PAUSE(sProtimer_t * const mobj, sEvent_t const * const e) {
    return eEvent_Ignored;
}

eEvent_Status STAT_TICK_TIME(sProtimer_t * const mobj, sEvent_t const * const e) {
    static uint8_t tick_count = 0;
    if(++tick_count == 40) {
        tick_count = 0;
        mobj->active_state = eProtimerState_Idle;
        return eEvent_Transition;
    }
    return eEvent_Ignored;
}

eEvent_Status STAT_ABRT(sProtimer_t * const mobj, sEvent_t const * const e) {
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