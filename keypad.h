#ifndef KEYPAD_H
#define KEYPAD_H


#define MAX_KEY_COL     4
#define MAX_KEY_ROW     4


void init_keypad(short * address_out, short * address_in) ;
int keypad_read(int * key_value);
int keyboard_read(int * key_value);

#endif
