#include "main.h"
#include "qpn.h"
#include "ClockAlarm_SM.h"
#include "button_SM.h"
Q_DEFINE_THIS_FILE;

static void SysTick_Init(void);
static void display_init(void);
static void attach_button_interrupts(void);

static QEvt ClockAlarmQueue[MAX_QUEUE_EVENT_NUMBER] = {0};
static QEvt ButtonQueue[MAX_QUEUE_EVENT_NUMBER] = {0};

QActiveCB const QF_active[] = 
{
    {(QActive *)0,(QEvt *)0,0},
    {(QActive *)AO_ClockAlarm,(QEvt *) ClockAlarmQueue,Q_DIM(ClockAlarmQueue)},
    {(QActive *)AO_Button,(QEvt *) ButtonQueue,Q_DIM(ButtonQueue)}
};

bool flag_report_button_press = true;

void setup() 
{
    Serial.begin(9600);
    Serial.println("Timer Test.!");
    display_init();

    pinMode(PIN_BUTTON1,INPUT);
    pinMode(PIN_BUTTON2,INPUT);
    attach_button_interrupts();
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, HIGH);

    Clock_Alarm_ctor();
    Button_ctor();
    QF_init(Q_DIM(QF_active));

}

void loop() 
{
    QF_run();
}

Q_NORETURN Q_onAssert (char_t const Q_ROM *const module, int_t const location)
{
  Serial.println("Assertion failed!!");
  Serial.println((String)module);
  Serial.println(location);
  while(1);
}

static void SysTick_Init(void)
{
    TCCR1A = TCCR1A_CTC_MODE;              //CTC mode            
    TCCR1B = (TCCR1B_CTC_MODE | TCCR1B_PRESCALER_1);         //prescaler=1,CTC mode
    TIMSK1 |= B00000010;                  //Interrupt enable for OCR1A compare match
    OCR1A = TIMER1_OC_MATCH_VALUE;          //OC match value for CONFIG_TICKS_PER_SECOND time base generation
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

void QF_onStartup(void)
{
  SysTick_Init();
}

void QV_onIdle(void)
{
    QV_CPU_SLEEP();
}

void SET_Handler(void)
{
    QF_INT_DISABLE();
    if(flag_report_button_press){
      flag_report_button_press = false;
      QActive_armX(AO_Button,0,MS_TO_TICKS(50),0U);
    }
    QF_INT_ENABLE();
}

void OK_Handler(void)
{
    QF_INT_DISABLE();
  if(flag_report_button_press){
    flag_report_button_press = false;
    QActive_armX(AO_Button,0,MS_TO_TICKS(50),0U);
  }
  QF_INT_ENABLE();
}

static void attach_button_interrupts(void)
{
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON1),SET_Handler,RISING);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON2),OK_Handler,RISING);
}

