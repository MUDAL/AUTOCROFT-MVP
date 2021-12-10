#ifndef _LCD_H
#define _LCD_H

extern void LCD_Init(void);
extern void LCD_PrintChar(char data);
extern void LCD_PrintString(char* pData);
extern void LCD_Clear(void);
extern void LCD_SetCursor(uint8_t row, uint8_t column);

#endif //_LCD_H
