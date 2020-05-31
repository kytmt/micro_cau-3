#include "keypad.h"
#include <stdio.h>
#include <string.h>


static short * keypad_out, * keypad_in;

char keyboard[16] = {'1', '2', '3', '4',
                     'q', 'w', 'e', 'r',
                     'a', 's', 'd', 'f',
                     'z', 'x', 'c', 'v'};

void init_keypad(short * address_out, short * address_in) {
        keypad_out  = address_out;
        keypad_in = address_in;
}

int keyboard_read(int * key_value){
        int key_count = 0;
        char buf[10];
	int i, j;
        scanf("%s", buf);
        key_count = strlen(buf);
        //printf("key_count : %d\n", key_count);
        if (key_count == 0){
                return key_count;
        }
        else{
                for( i = 0; i < key_count; i++ ){
                      for( j = 0; j < 16; j++ ){
                                if(buf[i] == keyboard[j]){
                                        *key_value = j;
                                }
                        }
                }
		return key_count;
        }
}

int keypad_read(int * key_value) {
        int col, row, key_count = 0;
        short key_temp;

        for( col=0; col<MAX_KEY_COL; col++ ) {
		*keypad_out = (short)(0x08 >> col);
                key_temp = * keypad_in;

                for( row=0; row<MAX_KEY_ROW; row++ ) {
                        if( ( (key_temp >> row) & 1 ) == 1 ) {
                                *key_value = (row * 4) + col ;
                                key_count++;
                        }
                }
        }
        return key_count;
}
