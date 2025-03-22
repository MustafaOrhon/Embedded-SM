#include <Arduino.h>

// ---------- ENUMS ----------
typedef enum {
    eProtimerSignal_First = 0,
    eProtimerSignal_IncTime = eProtimerSignal_First,
    eProtimerSignal_DecTime,
    eProtimerSignal_TickTime,
    eProtimerSignal_StartPause,
    eProtimerSignal_ABRT,
    eProtimerSignal_Entry,
    eProtimerSignal_Exit,
    eProtimerSignal_Last
} eProtimerSignal_t;

typedef enum {
    eProtimerState_First = 0,
    eProtimerState_Idle = eProtimerState_First,
    eProtimerState_Time_Set,
    eProtimerState_Countdown,
    eProtimerState_Pause,
    eProtimerState_Stat
} eProtimerState_t;

typedef enum {
    eEvent_Handled,
    eEvent_Ignored,
    eEvent_Transition
} eEvent_Status;

// ---------- STRUCTS ----------
typedef struct {
    uint32_t current_time;
    uint32_t elapsed_time;
    uint32_t proactive_time;
    eProtimerState_t active_state;
} sProtimer_t;

typedef struct {
    uint8_t signal;
} sEvent_t;

typedef struct {
    sEvent_t super;
} sProtimerUserEvent_t;

typedef struct {
    sEvent_t super;
    uint8_t ss;
} sProTimerTickEvent_t;

// ---------- FUNCTION DECLARATIONS ----------
void Protimer_Init(sProtimer_t *mobj);
eEvent_Status Protimer_StateMachine(sProtimer_t *const mobj, sEvent_t const *const e);
static eEvent_Status ProtimerStateHandler_Idle(sProtimer_t *const mobj, sEvent_t const *const e);
static void display_time(uint32_t time);
static void display_msg(String s);
static void display_clear(void);
static void do_beep(void);
static void protimer_event_dispatcher(sProtimer_t *const mobj, sEvent_t const *const e);

// ---------- GLOBAL OBJECT ----------
static sProtimer_t protimer;

// ---------- FUNCTION DEFINITIONS ----------
void Protimer_Init(sProtimer_t *const mobj) {
    mobj->active_state = eProtimerState_Idle;
    mobj->proactive_time = 0;
}

eEvent_Status Protimer_StateMachine(sProtimer_t *const mobj, sEvent_t const *const e) {
    switch (mobj->active_state) {
        case eProtimerState_Idle:
            return ProtimerStateHandler_Idle(mobj, e);
        default:
            return eEvent_Ignored;
    }
}

// ---------- STATE HANDLER ----------
static eEvent_Status ProtimerStateHandler_Idle(sProtimer_t *const mobj, sEvent_t const *const e) {
    switch (e->signal) {
        case eProtimerSignal_Entry:
            mobj->current_time = 0;
            display_time(0);
            display_msg("SET TIME");
            return eEvent_Handled;
        case eProtimerSignal_IncTime:
            mobj->current_time += 60;
            return eEvent_Handled;
        default:
            return eEvent_Ignored;
    }
}

// ---------- DISPLAY & HELPER FUNCTIONS ----------
static void display_time(uint32_t time) {
    Serial.print("Time: ");
    Serial.println(time);
}

static void display_msg(String s) {
    Serial.println(s);
}

static void display_clear(void) {
    Serial.println("Display Cleared");
}

static void do_beep(void) {
    Serial.println("Beep!");
}

// ---------- EVENT DISPATCHER ----------
static void protimer_event_dispatcher(sProtimer_t *const mobj, sEvent_t const *const e) {
    eEvent_Status status = Protimer_StateMachine(mobj, e);
}

// ---------- SETUP & LOOP ----------
void setup() {
    Serial.begin(9600);
    Protimer_Init(&protimer);
}

void loop() {
    sEvent_t exampleEvent;
    exampleEvent.signal = eProtimerSignal_IncTime;
    protimer_event_dispatcher(&protimer, &exampleEvent);
    delay(1000);
}
