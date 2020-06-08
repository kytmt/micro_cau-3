//-------| lib/main.h |-------//
#ifndef	MAIN_H
#define	MAIN_H 

#include <stdio.h>		// printf(), scanf(), fprintf()
#include <fcntl.h>		// open()
#include <unistd.h>		// close(), off_t , usleep()
#include <sys/mman.h>	// mmap(), munmap()
#include <stdlib.h> 	// exit()
#include <string.h>		// strlen()
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include "ieb.h"
#include "led.h"
#include "dot.h"
#include "fnd.h"
#include "clcd.h"
#include "keypad.h"


typedef struct image_data {
         unsigned char mainmenu         : 2;
         unsigned char temperature      : 2;
         unsigned char sidemenu         : 2;
         unsigned char flag             : 2;
} image_data_t;

typedef enum {
	FALSE = 0,
	TRUE  = 1
} truth_t;

typedef enum {
	ERROR	= -1,
	SUCCESS	= 0
} error_t;

typedef union {
	unsigned short all;
	struct {
		unsigned char	led  : 1;
		unsigned char 	clcd : 1;
		unsigned char 	dot  : 1;
		unsigned char 	fnd  : 1;
		unsigned char 	exit : 1;
		unsigned char    mainmenu : 2;
		unsigned char    sidemenu : 2;
		unsigned char     payment : 2;
		unsigned char temperature : 2;
		unsigned char      finish : 2;
		unsigned char	error : 1;
		unsigned char	flag : 1;
	};
} seclection_t;

off_t IEB_DOT[MAX_DOT] = {
        IEB_DOT1,
        IEB_DOT2,
        IEB_DOT3,
        IEB_DOT4,
        IEB_DOT5
};
off_t IEB_FND[MAX_FND] = {
        IEB_FND0,
        IEB_FND1,
        IEB_FND2,
        IEB_FND3,
        IEB_FND4,
        IEB_FND5,
        IEB_FND6,
        IEB_FND7
};

int main();
short * mapper(off_t offset, int prot);

truth_t logic();
void select_mode();
void input_mode();
void menu_display();
void print_usage();
void do_customer();
void do_owner();
short * mapper();
image_data_t* mapper_image();
void image_clear();
void emergency_closer();
void unmapper();
void sigint_handler();

#endif
