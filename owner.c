//***** Header file ****** //
#include <stdio.h>         
#include <stdlib.h>
#include <unistd.h>
#include <string.h>       // stdio.h~string.h are common header
#include <signal.h>       // signal, sigaction etc...
#include <time.h>         // time() 
#include <sys/types.h>

//******* Variable ******* //
typedef struct order {

	char mainmenu[20];
	char temperature[10];      //store information from customer process
	char sidemenu[20];
	char payment[10];

} order_t;	

static order_t o1;
static time_t sec_time;
static struct tm *cur_time;
static struct sigaction act;
static int order_num = 0;
static int americano_num = 0;
static int latte_num = 0;
static int hot_num = 0;
static int ice_num = 0;
static int bread_num = 0;
static int cookie_num = 0;


//***** Prototype ******* //
void sigusr1_handler(int sig, siginfo_t *info, void *ucontext);
void sigint_handler();
int sales_display();
void display_open();

//******* Code ********* //
int main(int argc, char **argv) 
{

	signal(SIGINT,sigint_handler);

	memset(&o1, 0, sizeof(struct order));        // initialization
	memset(&act, 0, sizeof(struct sigaction));   // initialization 
	act.sa_flags = SA_SIGINFO;                   // to use sa_sigaction as handler
	act.sa_sigaction = sigusr1_handler;          // set sigusr1_handler

	if (sigaction(SIGUSR1, &act, NULL)) {        // to receive payload and signal from customer
		printf("sigaction() fail\n");
		return -1;
	}

	display_open();

	while(1) {
		sleep(1);                           // to reduce CPU workload and to receive signal
	}

	return 0;

}

void display_open() 
{

        printf("\n************************************\n");
        printf("*                                  *\n");
        printf("*   CAU coffee shop sales record   *\n");
        printf("*                                  *\n");
        printf("************************************\n");

        system("echo Opening Times:; date ");
}	

void sigusr1_handler(int sig, siginfo_t *info, void *ucontext)
{
	memset(&o1, 0, sizeof(struct order));
        int temp;

        temp = info->si_int;                       // store data(payload) from customer process

	/* below code is used to extract data from payload */
	
        if      ((temp / 1000)  == 1) {
		strcpy(o1.payment, "Cash");
	}			
        else if ((temp / 1000) == 2) {
		strcpy(o1.payment, "Card");
	}
        temp %= 1000;

        if      ((temp / 100) == 1  ) {
		strcpy(o1.sidemenu, "Bread");
		bread_num += 1;
	}
        else if ((temp / 100) == 2  ) {
		strcpy(o1.sidemenu, "Cookie");
		cookie_num += 1;
	}
        temp %= 100;

        if      ((temp / 10) == 1) {
		strcpy(o1.temperature, "hot");
		hot_num += 1;
	}
        else if ((temp / 10) == 2) {
		strcpy(o1.temperature, "ice");
		ice_num += 1;
	}
        temp %= 10;

        if      (temp == 1) {
		strcpy(o1.mainmenu, "Americano");
		americano_num += 1;
	}
        else if (temp == 2) {
		strcpy(o1.mainmenu, "Latte");
		latte_num += 1;	
	}

	sales_display();                       // Display function on the terminal 
}

void sigint_handler() {

	int total;
	pid_t pid;

        printf("\n************************************\n");

        system("echo Closing Times:; date ");
	printf("\nTotal Order number : %d\n", order_num);
	printf("\n-- Sales stuffs --");
	printf("\n Americano : %d -> %d won", americano_num, 1500*americano_num);
        printf("\n Latte     : %d -> %d won", latte_num, 2000*latte_num);
        printf("\n hot       : %d -> free" , hot_num);
        printf("\n ice       : %d -> %d won", ice_num, 300*ice_num);
        printf("\n Bread     : %d -> %d won", bread_num, 3000*bread_num);
        printf("\n Cookie    : %d -> %d won\n", cookie_num, 2000*cookie_num);
	total = (1500*americano_num) + (2000*latte_num) + (300*ice_num) + (3000*bread_num) + (2000*cookie_num);
        printf("\n-- Total --"); 
        printf("\n Today sales : %d\n\n" , total);

	sleep(5);

	pid = getpid();
	kill(pid, SIGTERM);
 

}

int sales_display() {

	order_num += 1;
        sec_time = time(NULL);
        cur_time = localtime(&sec_time);

        if (!cur_time) {
                perror("localtime fail()\n");
                return -1;
        }

	printf("\n************************************");
	printf("\nOrder number : %d", order_num);
        printf("\nSelling times: %d/%d/%d %d:%d:%d",
                                        (*cur_time).tm_year+1900,
                                        (*cur_time).tm_mon+1,
                                        (*cur_time).tm_mday,
                                        (*cur_time).tm_hour,
                                        (*cur_time).tm_min,
                                        (*cur_time).tm_sec);
	printf("\nSales information :  %s %s %s %s\n", 
				o1.mainmenu,
				o1.temperature,
				o1.sidemenu,
				o1.payment);
	 
	return 0;
}
