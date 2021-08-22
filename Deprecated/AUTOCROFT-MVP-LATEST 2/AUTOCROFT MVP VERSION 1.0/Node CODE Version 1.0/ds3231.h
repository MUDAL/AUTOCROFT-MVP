#ifndef _DS3231_H
#define _DS3231_H

enum Period
{
	DS3231_PERIOD_AM = 0,
	DS3231_PERIOD_PM,
	DS3231_NO_PERIOD
};

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t period;
}ds3231_t;

extern void DS3231_Init(void);
extern void DS3231_GetTime(ds3231_t* pTime);
extern void DS3231_SetTime(uint8_t hour, uint8_t min);
extern void DS3231_12HourFormat(uint8_t periodOfDay);
extern void DS3231_24HourFormat(void);
extern void DS3231_SetAlarm2(uint8_t min);
extern bool DS3231_GetAlarm2_Status(void);
extern void DS3231_ResetAlarm2(void);

#endif //_DS3231_H
