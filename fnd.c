#include fnd.h

static unsigned short fnd_decimal[] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4f, // 3
	0x66, // 4
	0x6d, // 5
	0x7d, // 6
	0x07, // 7
	0x7f, // 8
	0x67, // 9
	}

static short * fnd[MAX_FND];
	
void fnd_clear() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0;
	}
}

void fnd_all() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0xFF;
	}
}

void fnd_write(int decimal, int fnd_num) {
	*fnd[fnd_num] = fnd_decimal[decimal];
}

void fnd_number(unsigned long number) {
	fnd_write( (short) (number %10), 0);
	fnd_write( (short) (number / 10 %10), 1);
	fnd_write( (short) (number / 100 %10), 2);
	fnd_write( (short) (number / 1000 %10), 3);
	
}

