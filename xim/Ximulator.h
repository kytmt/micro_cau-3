// Made by plaonn(plaonn@cau.ac.kr)

#ifndef _XIMULATOR_H
#define _XIMULATOR_H

/****************************************************************************\
|*                              Header                                      *|
\****************************************************************************/
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>	

/****************************************************************************\
|*                              Comment                                     *|
\****************************************************************************/

/****************************************************************************\
|*                              Macro                                       *|
\****************************************************************************/

/*********************************************|  Static Memory Map |*********/
#define SIM_MAX_RAND 10

#define SIM_SMM_CNT 17
static const off_t SIM_MMAP_SMM[SIM_SMM_CNT] = {
	0x11000000, // FND
	0x11100000,
	0x11200000,
	0x11300000,
	0x11400000,
	0x11500000,
	0x11600000,
	0x11700000,
	0x11800000, // DOT
	0x11900000,
	0x11A00000,
	0x11B00000,
	0x11C00000,
	0x12300000, // CLCD
	0x12380000,
	0x12400000,  // LED
	0X12500000  // IMAGE
};


/*********************************************|  CLCD              |*********/
#define SIM_CLCD_DD_RAM_MAX_CHAR	40
#define SIM_CLCD_DD_RAM_MAX_LINE	2
#define SIM_CLCD_MAX_CHAR			16
#define SIM_CLCD_MAX_LINE			2

/****************************************************************************\
|*                              Typedef                                     *|
\****************************************************************************/

typedef unsigned short vm_t;  // virtual memory type

/****************************************************************************\
|*                              Enum                                        *|
\****************************************************************************/


/****************************************************************************\
|*                              Struct                                      *|
\****************************************************************************/

/*********************************************|  CLCD              |*********/
typedef union {
	unsigned char CMD;
	unsigned char clrDisp;
	struct {
		unsigned char : 1;
		unsigned char retHome : 7;
	};
	struct {
		unsigned char S : 1;
		unsigned char ID : 1;
		unsigned char entrySet : 6;
	};
	struct {
		unsigned char B : 1;
		unsigned char C : 1;
		unsigned char D : 1;
		unsigned char dispCtrl : 5;
	};
	struct {
		unsigned char : 2;
		unsigned char RL : 1;
		unsigned char SC : 1;
		unsigned char shift : 4;
	};
	struct {
		unsigned char : 2;
		unsigned char F : 1;
		unsigned char N : 1;
		unsigned char DL : 1;
		unsigned char funcSet : 3;
	};
	struct {
		unsigned char CC_Address : 6;
		unsigned char set_CC : 2;
	};
	struct {
		unsigned char DD_Address : 7;
		unsigned char set_DD : 1;
	};
	struct {
		unsigned char D0 : 1;
		unsigned char D1 : 1;
		unsigned char D2 : 1;
		unsigned char D3 : 1;
		unsigned char D4 : 1;
		unsigned char D5 : 1;
		unsigned char D6 : 1;
		unsigned char D7 : 1;
	};
} clcd_cmd_t;

/*********************************************|  IMAGE             |*********/
typedef struct image_data {
	unsigned char mainmenu 	  : 2;
	unsigned char temperature : 2;
	unsigned char sidemenu    : 2;
	unsigned char flag	  : 2;
} image_data_t; 


/****************************************************************************\
|*                              Global                                      *|
\****************************************************************************/

/****************************************************************************\
|*                              Function Prototype                          *|
\****************************************************************************/

/*********************************************|  CLCD              |*********/
static void sim_clcd_clear();
static void sim_clcd_return();
static void sim_clcd_mode();
static void sim_clcd_control();
static void sim_clcd_shift();
static void sim_clcd_function();
static void sim_clcd_setCC();
static void sim_clcd_setDD();
static void sim_clcd_ctrl();
static void sim_clcd_view();
/*********************************************|  FND               |*********/
static void sim_fnd_view();

/*********************************************|  Dots & LED        |*********/
static void sim_dots_view();

/*********************************************|  IMAGE             |*********/
static void image_view();

/*********************************************|  Simulator Body    |*********/
static void simulator();

/*********************************************|  Wrapper           |*********/
int get_random_number();
int unget_random_number(int number);
int __real_usleep(useconds_t useconds);
int __wrap_usleep(useconds_t useconds);
int __wrap_open(const char *path, int oflag, ... );
int __wrap_close(int fildes);
void* __wrap_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
int __wrap_munmap(void *addr, size_t len);

/****************************************************************************\
|*                              Test Code                                   *|
\****************************************************************************/


#endif // _SIMULATOR_H
