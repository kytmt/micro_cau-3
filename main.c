//-------| src/main.c |-------//
#include "main.h"

static int fd;
static int map_counter = 0;
static int wstatus;
static pid_t pid;
static pid_t pid_owner;
static image_data_t *image;
void * map_data[100];
static seclection_t sel;
static union sigval sell;

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

void do_owner()
{
        signal(SIGINT, sigint_handler);
        system("./owner");
}

void do_customer()
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
        image = mapper_image(IEB_IMAGE, PROT_WRITE);  //additional code for Xim
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
void print_usage (char *prog)
{
        printf("%s (customer/owner) \n", prog);
}

void sigint_handler()
{
        kill(pid_owner, SIGINT);
        printf("\n");
        kill(getpid(), SIGTERM);
        pid = wait(&wstatus);
}

void image_clear() {

        /*initializing image_data*/

        image->mainmenu    = 0;
        image->temperature = 0;
        image->sidemenu    = 0;
        image->flag        = 0;
}


image_data_t * mapper_image(off_t offset, int prot) {
        map_data[map_counter] = mmap(NULL, sizeof(struct image_data), prot, MAP_SHARED, fd, offset);
        if ( map_data[map_counter] == MAP_FAILED ) {
                fprintf(stderr, "Cannot do mmap()");
                emergency_closer();
        }
        return (image_data_t *)map_data[map_counter++];
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

void select_mode() {
	int i;  char buf[100];
	char clcd_str_line1[20] = "";
	char clcd_str_line2[20] = "";

	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();
	image_clear();
	
	printf("\n");
	printf("************************************");
	printf("*******************\n");
	printf("*                                  \t\t      *\n");
	printf("*  \t    Welcome to the CAU coffee shop            *\n"); 
	printf("*                                  \t\t      *\n");
	printf("*     \t      Type any text to order       \t      *\n");
	printf("*                                  \t\t      *\n");
	printf("*******************");
	printf("************************************\n\n");
	scanf("%s", buf);

	if (strlen(buf) != 0) {
		sel.dot = 1;
		sel.mainmenu= 1;
	}
	sell.sival_int = 0;
	sel.flag=0;
	menu_display();
}



void menu_display() {

        int stage = 0; 
	int i = 0;
	char clcd_str_line1[20] = "";
        char clcd_str_line2[20] = "";

        if( sel.mainmenu == 1 )         { clcd_write_menu(i); }
        else if( sel.temperature == 1)  { 
		clcd_write_menu(i+2); 
		stage = 1;
		led_stage(stage);	
	}
        else if( sel.sidemenu == 1 )    { 
		clcd_write_menu(i+4);
		stage = 2; 
		led_stage(stage);
	}
        else if( sel.payment == 1)      { 
		clcd_write_menu(i+6);
		stage = 3; 
		led_stage(stage);
	}
        else if( sel.finish == 1) {

                 clcd_write_menu(i+8);
                sigqueue(pid_owner, SIGUSR1, sell);

                sleep(5);
		image->flag = 2; //image intializing
                sel.all = 0;
        }

        else if (sel.error == 1)  {

                if (sel.flag) {
                        clcd_write_menu(i+10);

                        sleep(3);
			image->flag = 2; //image intializing
                        sel. all =0;
                } else {

                        clcd_write_menu(i+12);
                        sleep(3);
			image -> flag = 2; //image intializing
                        sel.all = 0;
                }
        }
}

void input_mode() {
	int key_count, key_value;
	char clcd_str_line1[20] = "";
	char clcd_str_line2[20] = "";
	int number;
	//key_count = keypad_read(&key_value);
	key_value = keyboard_read(&key_value, &key_count);
	
	dot_write(key_value+1); //dot write
	if(key_value == 0) {
	// next page choose 1//
			if (sel.mainmenu == 1) {
					 sel.all =0;  // initialize
				sel.temperature = 1;  // set temperature 1
				sell.sival_int += 1;  // payload for owner
			             menu_display();
				image->mainmenu = 1;  // mapped data image_data_t
				      number = 1500;  // choose americano
				 fnd_number(number);  // display fnd
			}
                       
			else if (sel.temperature == 1) {
                                          sel.all=0;  // intialize	 
				   sel.sidemenu = 1;  // set sidemenu 1
                               sell.sival_int += 10;  // payload for owner
			     image->temperature = 1;  // mapped data image_data_t
			             menu_display();
				 fnd_number(number);  // display fnd
                        }
			
			else if (sel.sidemenu == 1) {
                               	          sel.all=0;  //same above     
				    sel.payment = 1;
                              sell.sival_int += 100;
				     menu_display();
				image->sidemenu = 1;

				if ( sel.flag == 1) { number = 3000; } // jump mainmenu and choose bread 
				else	            { number += 3000;} // choose bread   
				 fnd_number(number);
			}

                        else if (sel.payment == 1) { 
					  sel.all=0; //same above
			             sel.finish = 1;
                             sell.sival_int += 1000; 
			 	     menu_display();
				 fnd_number(number);
                        }
	}

	else if ( key_value == 1) {
	// next page choose 2//
	                if (sel.mainmenu == 1) {
				   	  sel.all=0; //same above
                                sel.temperature = 1;
				sell.sival_int += 2;
				     menu_display();
				image->mainmenu = 2;
				      number = 2000; //chose latte
				 fnd_number(number);
			}

                        else if (sel.temperature == 1) {
				         sel.all =0; //same above
                                   sel.sidemenu = 1;
                               sell.sival_int += 20;
				     menu_display();
			     image->temperature = 2;
			              number += 300; //choose ice 
				 fnd_number(number);
                        }

                        else if (sel.sidemenu == 1) {
					  sel.all=0; //sama above
                                    sel.payment = 1;
                              sell.sival_int += 200;
			  	     menu_display();
				image->sidemenu = 2;
                                if ( sel.flag == 1) { number = 2000; } //jump mainmenu and choose cookie
                                else     	    { number += 2000;} //choose cookie 
				 fnd_number(number);
                        }
                        else if (sel.payment == 1) {
					  sel.all=0; //same above
				     sel.finish = 1;
                              sell.sival_int +=2000;
				     menu_display();
				 fnd_number(number);
                        }

	
	}

	else if ( key_value == 2) {
	// back to previous stage //
			if (sel.flag == 1)  {
				sel.all = 0; 
				sel.flag = 0;
				sel.error = 1;
				menu_display();
			}
			else if (sel.temperature == 1) {

		                sel.all=0;   
				sel.mainmenu = 1;
				sell.sival_int = 0;

				if(image->mainmenu == 1) {
					number -= 1500;		
					fnd_number(number);
				} else if (image ->mainmenu == 2){
					number -= 2000;
					fnd_number(number);
				}		
			             menu_display();
			}
                        else if (sel.sidemenu == 1) {
				sel.all=0;
                                sel.temperature = 1;
				sell.sival_int %= 10;

                                if(image->temperature == 2) {
                                        number -= 300; 
                                        fnd_number(number);
                                }
			             menu_display();
			}
			else if (sel.payment == 1) {
					sel.all=0;
                                   sel.sidemenu = 1;
				sell.sival_int %= 100;

                                if(image->sidemenu == 1) {
                                        number -= 3000; 
                                        fnd_number(number);
                                } else if (image ->sidemenu == 2){
                                        number -= 2000;
                                        fnd_number(number);
                                }

                                     menu_display();
                        }
	}

	else if ( key_value == 3) {
        // no choice //

			if (sel.mainmenu == 1) {
				sel.all=0;
				sel.sidemenu = 1;
				sel.flag = 1;     // flag means skipping sel.mainmenu // 
				image->flag = 1;  //send data image_data_t about jumping //
				menu_display();
			}
                        else if (sel.sidemenu == 1 ) {
				if (sel.flag) {
					sel.all=0;
					sel.error = 1;
					menu_display();
				} else {
					sel.all=0;
                                    sel.payment = 1;
                                     menu_display();
				}
                        }
        }
	else {
	// restart //
		sel.all = 0;
		sel.error = 1;
		menu_display();
	}
  	
	if ( key_count > 1 ) {
		sel.all = 0;
	} 
	usleep(0);
}


