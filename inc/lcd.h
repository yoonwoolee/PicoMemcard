#ifndef __LCD_H__
#define __LCD_H__

void lcd_string(const char *s);
void lcd_clear(void) ;
void lcd_set_cursor(int line, int position);

#endif