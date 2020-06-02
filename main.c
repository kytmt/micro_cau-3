//-------| src/main.c |-------//
#include "main.h"
#include <sys/wait.h>

static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};
static off_t IEB_FND[MAX_FND] = {
	IEB_FND0,
	IEB_FND1,
	IEB_FND2,
	IEB_FND3,
	IEB_FND4,
	IEB_FND5,
	IEB_FND6,
	IEB_FND7
};

static int fd;
static int map_counter = 0;
static void * map_data[100];
static seclection_t sel;
static pid_t pid;
static pid_t pid_owner; 
static int wstatus;

int main(int argc, char* argv[]) 
{

        if (argc != 2) {
		print_usage(argv[0]);
		return -1;
        }

	pid  = fork();

	if ( (pid ==0) && !strcmp(argv[1], "owner")) {
		/* child process for owner */
		do_owner();		

	} else if ( (pid > 0) && !strcmp(argv[1], "customer")) {
		/* parent process for customer*/
		do_customer();

	} else {

	        if (strcmp(argv[1], "owner")) {
			print_usage(argv[0]);
			return -1;
		}
	}

	pid = wait(&wstatus); // zoombie child process protection

	return 0;
}

static void do_owner()
{
        signal(SIGINT, sigint_handler);
	system("./owner");
}

static void do_customer()
{
	/***** additional code *****/
        char line[100];

        FILE *cmd = popen("pidof owner", "r");

        fgets(line, 100, cmd);
        pid_owner = strtoul(line, NULL, 10);     // get pidof owner 
        signal(SIGINT, sigint_handler);          // set SIGINT(CTRL+C) handler

	/***** Default code *****/

        int i;
        short * led, * dot[MAX_DOT], * fnd[MAX_FND];
        short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in;
	
        fd = open("/dev/mem", O_RDWR|O_SYNC);
        if (fd == -1) {
                fprintf(stderr, "Cannot open /dev/mem file");
                exit(EXIT_FAILURE);
        }

        led = mapper(IEB_LED, PROT_WRITE);
        for( i=0; i<MAX_DOT; i++ ) {
                dot[i] = mapper(IEB_DOT[i], PROT_WRITE);
        }
        for( i=0; i<MAX_FND; i++ ) {
                fnd[i] = mapper(IEB_FND[i], PROT_WRITE);
        }
        clcd_cmd  = mapper(IEB_CLCD_CMD, PROT_WRITE);
        clcd_data = mapper(IEB_CLCD_DATA, PROT_WRITE);
        //keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);
        //keypad_in = mapper(IEB_KEY_R, PROT_READ);

        init_led(led);
        init_dot(dot);
        init_fnd(fnd);
        init_clcd(clcd_cmd, clcd_data);
        //init_keypad(keypad_out, pid_ownerkeypad_in);

        sel.all = 0;
        while( logic() == TRUE ) {      }

        unmapper();
        close(fd);
	pclose(cmd);           // pclose popen 
}


static void sigint_handler()
{
	kill(pid_owner, SIGINT);
	printf("\n");
        kill(getpid(), SIGTERM);
	pid = wait(&wstatus);
}


short * mapper(off_t offset, int prot) {
	map_data[map_counter] = mmap(NULL, sizeof(short), prot, MAP_SHARED, fd, offset);
	if ( map_data[map_counter] == MAP_FAILED ) {
		fprintf(stderr, "Cannot do mmap()");
		emergency_closer();
	}
	return (short *)map_data[map_counter++];
}

void unmapper() {
	int i;
	for( i=0; i<map_counter; i++) {
		munmap(map_data[i], sizeof(short));
	}
	map_counter = 0;
}

void emergency_closer() {
	unmapper();
	close(fd);
	exit(EXIT_FAILURE);
}

truth_t logic() {
	if( sel.all == 0 ) { select_mode(); }
	else if( sel.exit == 1 ) { return FALSE; }
	else { input_mode(); }
	return TRUE;
}

void print_usage (char *prog)
{
        printf("%s (customer/owner) \n", prog);
}


void select_mode() {
	int i;  char buf[100];
	char clcd_str_line1[20] = "";
	char clcd_str_line2[20] = "";

	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();
	
	printf("\n");
	printf("************************************\n");
	printf("*                                  *\n");
	printf("*  Welcome to the CAU coffee shop  *\n"); 
	printf("*                                  *\n");
	printf("*     Type any text to order       *\n");
	printf("*                                  *\n");
	printf("************************************\n\n");
	scanf("%s", buf);
	if (strlen(buf) != 0) {
		sel.mainmenu= 1;
	}
        sel.sidemenu = 0;
        sel.payment = 0;
        sel.temperature = 0;
        sel.finish = 0;
	sell.sival_int = 0;

	menu_display();
}

void menu_display() {
        
	char clcd_str_line1[20] = "";
        char clcd_str_line2[20] = "";


        if( sel.mainmenu == 1 ) {

        clcd_clear_display();
        memset(clcd_str_line1, 0, sizeof(clcd_str_line1));
        memset(clcd_str_line2, 0, sizeof(clcd_str_line2));

        strcat(clcd_str_line1, "1.Americano 1500");
        clcd_write_string(clcd_str_line1);
        clcd_set_DDRAM(0x40);
        strcat(clcd_str_line2, "2.Latte     2000");
        clcd_write_string(clcd_str_line2);
	
        }

	if( sel.temperature == 1) {
        clcd_clear_display();
        memset(clcd_str_line1, 0, sizeof(clcd_str_line1));
        memset(clcd_str_line2, 0, sizeof(clcd_str_line2));

        strcat(clcd_str_line1, "1.hot   ");
        clcd_write_string(clcd_str_line1);
        clcd_set_DDRAM(0x40);
        strcat(clcd_str_line2, "2.ice    +300");
        clcd_write_string(clcd_str_line2);
		stage=1;
        led_stage();
	}

        if( sel.sidemenu == 1 ) {
	
	clcd_clear_display();
        memset(clcd_str_line1, 0, sizeof(clcd_str_line1));
        memset(clcd_str_line2, 0, sizeof(clcd_str_line2));

        strcat(clcd_str_line1, "1.Bread     3000");
        clcd_write_string(clcd_str_line1);
        clcd_set_DDRAM(0x40);
        strcat(clcd_str_line2, "2.Cookie    2000");
        clcd_write_string(clcd_str_line2);
	    stage=2;
        led_stage();

        }

	if( sel.payment == 1) {
        
	clcd_clear_display();
        memset(clcd_str_line1, 0, sizeof(clcd_str_line1));
        memset(clcd_str_line2, 0, sizeof(clcd_str_line2));

        strcat(clcd_str_line1, "1.Cash ");
        clcd_write_string(clcd_str_line1);
        clcd_set_DDRAM(0x40);
        strcat(clcd_str_line2, "2.Card");
        clcd_write_string(clcd_str_line2);
		stage=3;
  	    led_stage();
	}

	if( sel.finish == 1) {
        clcd_clear_display();
        memset(clcd_str_line1, 0, sizeof(clcd_str_line1));
        memset(clcd_str_line2, 0, sizeof(clcd_str_line2));

        strcat(clcd_str_line1, "Order completed ");
        clcd_write_string(clcd_str_line1);
        clcd_set_DDRAM(0x40);
        strcat(clcd_str_line2, "Please wait ^~^");
        clcd_write_string(clcd_str_line2);

        sigqueue(pid_owner, SIGUSR1, sell);
	
	sleep(5);
	sel.all = 0;
	} 
}

void input_mode() {
	int key_count, key_value;
	char clcd_str_line1[20] = "";
	char clcd_str_line2[20] = "";

	//key_count = keypad_read(&key_value);
	key_value = keyboard_read(&key_value, &key_count);
	if(key_value == 0) {
	// next page choose 1//
			if (sel.mainmenu == 1) {
			           sel.mainmenu = 0;
				   sel.sidemenu = 0;
				    sel.payment = 0;
				sel.temperature = 1;
				     sel.finish = 0;
				sell.sival_int += 1;		
			             menu_display();
					sel.all = 1;
					number = 1500;
					fnd_number(number);
			}
                       
			else if (sel.temperature == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 1;
                                    sel.payment = 0;
                                sel.temperature = 0;
                                     sel.finish = 0;
                                sell.sival_int += 10;
				menu_display();
								 fnd_number(number);

                        }
			
			else if (sel.sidemenu == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 0;
                                    sel.payment = 1;
                                sel.temperature = 0;
                                     sel.finish = 0;
                                sell.sival_int += 100;
									menu_display();
									 number += 3000;
									fnd_number(number);

			}
                        else if (sel.payment == 1) { 
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 0;
                                    sel.payment = 0;
                                sel.temperature = 0;
			             sel.finish = 1;
                                sell.sival_int += 1000;
								menu_display();
								 fnd_number(number);


                        }
 
	}

	else if ( key_value == 1) {
	// next page choose 2//
	                if (sel.mainmenu == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 0;
                                    sel.payment = 0;
                                sel.temperature = 1;
                                     sel.finish = 0;
								sell.sival_int += 2;
								menu_display();
								sel.all = 1;
								number = 2000;
								fnd_number(number);
			}

                        else if (sel.temperature == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 1;
                                    sel.payment = 0;
                                sel.temperature = 0;
                                     sel.finish = 0;
                                sell.sival_int += 20;
									menu_display();
									 number += 300;
									 fnd_number(number);
                        }

                        else if (sel.sidemenu == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 0;
                                    sel.payment = 1;
                                sel.temperature = 0;
                                     sel.finish = 0;
                                sell.sival_int += 200;
								menu_display();
									 number += 2000;
									 fnd_number(number);

                        }
                        else if (sel.payment == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 0;
                                    sel.payment = 0;
                                sel.temperature = 0;
									sel.finish = 1;
                                sell.sival_int += 2000;
								menu_display();
									 fnd_number(number);

                        }

	
	}

	else {
	// back to the mainmenu //
						if (sel.temperature == 1) {
		                   sel.mainmenu = 1;
                                   sel.sidemenu = 0;
                                    sel.payment = 0;
                                sel.temperature = 0;
								sel.finish = 0;
								sell.sival_int = 0;		
								menu_display();
			}
                        if (sel.sidemenu == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 0;
                                    sel.payment = 0;
                                sel.temperature = 1;
									sel.finish = 0;
								sell.sival_int %= 10;
								menu_display();
			}
						if (sel.payment == 1) {
                                   sel.mainmenu = 0;
                                   sel.sidemenu = 1;
                                    sel.payment = 0;
                                sel.temperature = 0;
									sel.finish = 0;
								sell.sival_int %= 100;
                                     menu_display();
                        }
	} 
  	
	if ( key_count > 1 ) {
		sel.all = 0;
	} 
	usleep(0);
}


