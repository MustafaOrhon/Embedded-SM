#include "lcd.hpp"
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(0x27,16,2);

void lcd_init(void)
{
    lcd.init();
    lcd.backlight();
}
void lcd_clear(void)
{
    lcd.clear();
}
void lcd_print_char(char c)
{
    lcd.print(c);
}
void lcd_scroll_left(void)
{
    lcd.scrollDisplayLeft();
}
void lcd_scroll_right(void)
{
    lcd.scrollDisplayRight();
}
void lcd_set_cursor(int c,int r)
{
    lcd.setCursor(c,r);
}
void lcd_no_auto_scroll(void)
{
    lcd.noAutoscroll();   
}
void lcd_move_cursor_R_to_L(void)
{
    lcd.rightToLeft();
}
void lcd_move_cursor_L_to_R(void)
{
    lcd.leftToRight();
}
void lcd_cursor_off(void)
{
    lcd.noCursor();
}
void lcd_cursor_blinkoff(void)
{
    lcd.noBlink();
}
void lcd_insert_number(int num)
{
    lcd.print(num);
}
void lcd_print_string(String s)
{
    lcd.print(s);
}

void lcd_cursor_off(void)
{
    lcd.noCursor();
}

void lcd_cursor_show(void)
{
    lcd.cursor();
}

void lcd_cursor_blink(void)
{
    lcd.blink();
}

void lcd_create_custom_char(byte num, byte* data) {
    lcd.createChar(num, data);
}

void lcd_display_custom_char(byte num){
    lcd.write(num);
}