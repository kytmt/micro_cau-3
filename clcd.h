//-------| lib/clcd.h |-------//
#ifndef	CLCD_H
#define	CLCD_H 


#include <unistd.h> // usleep()
#include <string.h> // memset()

#define CLCD_CMD_US		40
#define CLCD_DATA_US	(CLCD_CMD_US + 4)
#define CLCD_RETURN_US	(1520 - CLCD_CMD_US)

static char *menu[] = {
               "1.Americano 1500",
               "2.Latte     2000",
               "1.hot"          ,
               "2.ice    +300",
               "1.Bread     3000",
               "2.Cookie    2000",
               "1.Cash",
               "2.Card",
               "Order completed",
               "Please wait ^~^",
               "Please Order   ",
               "at least 1 menu",
               "Invalid input",
               "Restart "
};


void init_clcd(short * address_cmd, short * address_data);
void clcd_write_cmd(int cmd);
void clcd_write_data(int data);
void clcd_clear_display();
void clcd_return_home();
void clcd_entry_mode_set(int ID, int S);
void clcd_display_control(int D, int C, int B);
void clcd_shift(int SC, int RL);
void clcd_function_set(int DL, int N, int F);
void clcd_set_DDRAM(int address);
void clcd_write_string(char str[]);
void clcd_write_menu(int i);

#endif
