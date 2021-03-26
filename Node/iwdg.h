#ifndef IWDG_H
#define IWDG_H

//Starts the IWDG watchdog
extern void IWDG_Start(void);
/*Reloads the IWDG watchdog's counter to prevent
a system reset.
*/
extern void IWDG_Refresh(void);

#endif /* IWDG_H */
