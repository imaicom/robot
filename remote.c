// cc -o remote remote.c -lwiringPi -lm
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include <wiringPi.h>
#include <softPwm.h>

#include "controller.h"
#include <math.h>


struct ps3ctls {
	
	int fd;
	unsigned char nr_buttons;	// Max number of Buttons
	unsigned char nr_sticks;	// Max number of Sticks
	short *button;			// button[nr_buttons]
	short *stick;			// stick[nr_sticks]
};

int fds;
#define saki3o 47
#define saki4o 52
#define saki5o 3
int saki1  = 0;
int saki2  = 0;
int saki3  = saki3o;
int saki4  = saki4o;
int saki5  = saki5o;
int mode = 10;
int btn_tri = 0;
int b_btn_tri = 0;
int btn_cir = 0;
int b_btn_cir = 0;
int btn_up = 0;
int b_btn_up = 0;
int btn_down = 0;
int b_btn_down = 0;
int a_mode = 0;
int b_mode = 0;

int servo00 = 0;
int servo01 = 0;
int servo02 = 0;

int servo03 = 40;
int servo04 = 130;
int servo04b = 0;
int servo05 = 28;
int servo05b = 0;
int servo06 = -45;

int resetPCA9685(int fd) {
	wiringPiI2CWriteReg8(fd,0,0);
}

int setPCA9685Freq(int fd , float freq) {
	float prescaleval;
	int prescale , oldmode , newmode;
	freq = 0.9 * freq;
	prescaleval = 25000000.0;
	prescaleval /= 4096.0;
	prescaleval /= freq;
	prescaleval -= 1.0;
	prescale = prescaleval + 0.5;
	oldmode = wiringPiI2CReadReg8(fd,0x00);
	newmode = (oldmode & 0x7F)|0x10;
	wiringPiI2CWriteReg8(fd , 0x00 , newmode);
	wiringPiI2CWriteReg8(fd , 0xFE , prescale);
	wiringPiI2CWriteReg8(fd , 0x00 , oldmode);
	sleep(0.005);
	wiringPiI2CWriteReg8(fd , 0x00 , oldmode | 0xA1);
}


int setPCA9685Duty(int fd , int channel , int off) {
	int channelpos;
	int on;
	
	on   = 0;
	off += 276;
	channelpos = 0x6 + 4 * channel;
	wiringPiI2CWriteReg16(fd , channelpos   , on  & 0x0FFF);
	wiringPiI2CWriteReg16(fd , channelpos+2 , off & 0x0FFF);
}

int ps3c_test(struct ps3ctls *ps3dat) {

	int i;
	unsigned char nr_btn = ps3dat->nr_buttons;
	unsigned char nr_stk = ps3dat->nr_sticks;
	int xx,yy,x,y,z;
	
//	printf("%d %d\n",nr_btn,nr_stk);

//  	printf(" 1=%2d ",ps3dat->button[PAD_KEY_LEFT]);
//	printf(" 2=%2d ",ps3dat->button[PAD_KEY_RIGHT]);
//	printf(" 3=%2d ",ps3dat->button[PAD_KEY_UP]);
//	printf(" 4=%2d ",ps3dat->button[PAD_KEY_DOWN]);
//	printf(" 5=%4d ",ps3dat->stick [PAD_LEFT_X]);
//	printf(" 6=%4d ",ps3dat->stick [PAD_LEFT_Y]);
//	printf(" 7=%4d ",ps3dat->stick [PAD_RIGHT_X]);
	printf(" 03=%4d ",servo03);
	printf(" 04=%4d ",servo04);
	printf(" 04b=%4d ",servo04b);
	printf(" 05=%4d ",servo05);
	printf(" 05b=%4d ",servo05b);
	printf(" 06=%4d ",servo06);
	printf(" mode=%2d ",mode);
	printf(" a_mode=%2d ",a_mode);
	printf(" b_mode=%2d ",b_mode);
	printf(" btn_tri=%2d ",btn_tri);
	//	printf(" 8=%4d ",ps3dat->stick [PAD_RIGHT_Y]);
	printf("\n"); 

	xx = ps3dat->stick [PAD_LEFT_X];
	yy = ps3dat->stick [PAD_LEFT_Y];
	xx = 0; // imai
	yy = 0; // imai
	x = -xx * cos(-M_PI/4) - yy * sin(-M_PI/4);
	y = -xx * sin(-M_PI/4) + yy * cos(-M_PI/4);

	if(abs(x) < 5) {
		softPwmWrite(26,0);
		softPwmWrite(27,0);
	} else if(x > 0) {
		softPwmWrite(26,0);
		softPwmWrite(27,abs(x));
	} else {
		softPwmWrite(26,abs(x));
		softPwmWrite(27,0);
	};

	if(abs(y) < 5) {
		softPwmWrite(5,0);
		softPwmWrite(6,0);
	} else if(y > 0) {
		softPwmWrite(5,0);
		softPwmWrite(6,abs(y));
	} else {
		softPwmWrite(5,abs(y));
		softPwmWrite(6,0);
	};

	
	z = ps3dat->stick [PAD_RIGHT_X];

	if(abs(z) < 5) {
		softPwmWrite(28,0);
		softPwmWrite(29,0);
		softPwmWrite(24,0);
		softPwmWrite(25,0);
	} else if(z > 0) {
		softPwmWrite(28,0);
		softPwmWrite(29,abs(z));
		softPwmWrite(24,abs(z));
		softPwmWrite(25,0);
	} else {
		softPwmWrite(28,abs(z));
		softPwmWrite(29,0);
		softPwmWrite(24,0);
		softPwmWrite(25,abs(z));

	};


//	setPCA9685Duty(fds , 0 , ps3dat->stick [PAD_RIGHT_X]);
//	setPCA9685Duty(fds , 1 , ps3dat->stick [PAD_RIGHT_X]);

//	if(ps3dat->button[PAD_KEY_CROSS]==1) {
//		softPwmWrite(28,0);
//		softPwmWrite(29,0);
//		softPwmWrite(24,0);
//		softPwmWrite(25,0);
//		return -1; // end of program
//	};

/*
	if(ps3dat->button[PAD_KEY_TRIANGLE]) 	{ servo00++; };
	if(ps3dat->button[PAD_KEY_CROSS]) 		{ servo00--; };
	setPCA9685Duty(fds , 0 , servo00);
*/

//	if(ps3dat->button[PAD_KEY_UP]) { servo03++; };
//	if(ps3dat->button[PAD_KEY_DOWN]) { servo03--; };
//	setPCA9685Duty(fds , 3 , servo03);

//	if(ps3dat->button[PAD_KEY_LEFT]) { servo04++; };
//	if(ps3dat->button[PAD_KEY_RIGHT]) { servo04--; };
//	setPCA9685Duty(fds , 4 , servo04);

//	if(ps3dat->button[PAD_KEY_UP]) { servo06++; };
//	if(ps3dat->button[PAD_KEY_DOWN]) { servo06--; };
//	setPCA9685Duty(fds , 6 , servo06);
	
	if(!ps3dat->button[PAD_KEY_L1] && !ps3dat->button[PAD_KEY_L2]) {
		servo05 = 28 - servo05b;
	} else if( ps3dat->button[PAD_KEY_L1] && !ps3dat->button[PAD_KEY_L2])	 {
		servo05++;
		servo05b = 0;
	} else if(!ps3dat->button[PAD_KEY_L1] &&  ps3dat->button[PAD_KEY_L2])	 {
		servo05--;
		servo05b = +6;
	};
	setPCA9685Duty(fds , 5 , servo05);
	

	if(ps3dat->button[PAD_KEY_SQUARE]) {softPwmWrite(3,50);} else {softPwmWrite(3,0);};

//	!! Danger !!
//	if(ps3dat->button[PAD_KEY_TRIANGLE]) btn_tri++;
//	if(!ps3dat->button[PAD_KEY_TRIANGLE]) btn_tri = 0;
//	if(b_btn_tri > btn_tri) {mode++;if(mode > 3) mode = 0;};
//	b_btn_tri = btn_tri;
	
	if(ps3dat->button[PAD_KEY_CIRCLE]) btn_cir++;
	if(!ps3dat->button[PAD_KEY_CIRCLE]) btn_cir = 0;
	if(b_btn_cir > btn_cir) {
		if(mode>=10) {mode++;if(mode>11) mode=10;};
		if((0<=mode)&&(mode <10)) {mode = 10;};
	};
	b_btn_cir = btn_cir;

	if(ps3dat->button[PAD_KEY_UP]) btn_up++;
	if(!ps3dat->button[PAD_KEY_UP]) btn_up = 0;
	if(b_btn_up > btn_up) {
		a_mode++;if(a_mode > 12) a_mode = 0;
		if(a_mode == 1) {softPwmWrite(3,50);delay(200);softPwmWrite(3,0);};
		if(a_mode == 5) {softPwmWrite(3,50);delay(200);softPwmWrite(3,0);};
		if(a_mode == 8) {softPwmWrite(3,50);delay(200);softPwmWrite(3,0);};
	};
	b_btn_up = btn_up;
	
	if(ps3dat->button[PAD_KEY_DOWN]) btn_down++;
	if(!ps3dat->button[PAD_KEY_DOWN]) btn_down = 0;
	if(b_btn_down > btn_down) {b_mode++;if(b_mode > 1) b_mode = 0;};
	b_btn_down = btn_down;
	
	if( ps3dat->button[PAD_KEY_R1] ) servo04b++;
	if( ps3dat->button[PAD_KEY_R2] ) servo04b--;
	
	if(b_mode == 1) {
		servo06 = -128;
	};
	if(b_mode == 0) {
		servo06 = -45;
	};
	
	if(a_mode == 1) {
		servo03 = 4;
		servo04 = 41 - servo04b;
	};
	if(a_mode == 2) {
		servo03 = 15;
		servo04b = 0;
	};
	if(a_mode == 3) {
		servo03 = 24;
		servo04 = -17;
		servo04b = 0;
	};
	if(a_mode == 4) {
		servo03 = 35;
		servo04 = 100;
		servo04b = 0;
	};
	if(a_mode == 5) {
		servo03 = 10;
		servo04 = -60 - servo04b;
	};
	if(a_mode == 6) {
		servo03 = 40;
		servo04 = -17;
		servo04b = 0;
	};
	if(a_mode == 7) {
		servo04 = 130;
		servo04b = 0;
	};
	if(a_mode == 8) {
		servo03 = 10;
		servo04 = -60 - servo04b;
	};
	if(a_mode == 9) {
		servo03 = 20;
		servo04 = 20;
		servo04b = 0;
	};
	if(a_mode == 10) {
		servo03 = 40;
		servo04 = 130;
		servo04b = 0;
		a_mode = 0;
	};
	
	setPCA9685Duty(fds , 3 , servo03);
	setPCA9685Duty(fds , 4 , servo04);
	setPCA9685Duty(fds , 5 , servo05);
	setPCA9685Duty(fds , 6 , servo06);

	
	if(mode == 10) {
		setPCA9685Duty(fds , 0 , +126);
		setPCA9685Duty(fds , 1 , +110);
		setPCA9685Duty(fds , 2 , +120);
	};
	if(mode == 11) {
		for (i=0;i<126;i++) {
			setPCA9685Duty(fds , 0 , 126-i);
			setPCA9685Duty(fds , 1 , 110-i);
			delay(20);
		};
		mode = 0;
//		setPCA9685Duty(fds , 0 , +126);
//		setPCA9685Duty(fds , 1 , +110);
//		setPCA9685Duty(fds , 0 ,  0);
//		setPCA9685Duty(fds , 1 , 10);
		setPCA9685Duty(fds , 2 , +120);
	};

	if(mode == 0) {
		setPCA9685Duty(fds , 0 ,  0);
		setPCA9685Duty(fds , 1 , 10);
		setPCA9685Duty(fds , 2 , +120);
	};
	if(mode == 1) {
		setPCA9685Duty(fds , 0 , -70);
		setPCA9685Duty(fds , 1 , -80);
		setPCA9685Duty(fds , 2 , 0);
		softPwmWrite(28,0);
		softPwmWrite(29,20);
		softPwmWrite(24,0);
		softPwmWrite(25,20);
		delay(1500);
		
		setPCA9685Duty(fds , 2 , +120);
		softPwmWrite(28,0);
		softPwmWrite(29,0);
		
		softPwmWrite(24,20);
		softPwmWrite(25,0);
		delay(400);
		softPwmWrite(24,0);
		softPwmWrite(25,0);

		mode = 2;
	};

	if(mode == 2) {
	};
	
	if(mode == 3) {
		setPCA9685Duty(fds , 2 , 20);
		delay(2000);
		softPwmWrite(24,10);
		softPwmWrite(25,0);


		for(i=0;i<40;i++) {
			setPCA9685Duty(fds , 0 , i-70);
			setPCA9685Duty(fds , 1 , i-80);
			delay(40);
//			setPCA9685Duty(fds , 0 , -70);
//			setPCA9685Duty(fds , 1 , -80);		
//			setPCA9685Duty(fds , 0 ,  0);
//			setPCA9685Duty(fds , 1 , 10);
		};
		
		softPwmWrite(24,0);
		softPwmWrite(25,0);
		
		
		for(i=20;i<120;i++) {
			setPCA9685Duty(fds , 2 , i);
			delay(50);
		};
		mode = 0;
		setPCA9685Duty(fds , 0 ,  0);
		setPCA9685Duty(fds , 1 , 10);
		setPCA9685Duty(fds , 2 , +120);
	};

//	if(ps3dat->button[PAD_KEY_CIRCLE]) {
//		mode = 0;
//	};


	return 0;
}


int ps3c_input(struct ps3ctls *ps3dat) {

	int rp;
	struct js_event ev;

	do {
		rp = read(ps3dat->fd, &ev, sizeof(struct js_event));
		if (rp != sizeof(struct js_event)) {
			return -1;
		}
	} while (ev.type & JS_EVENT_INIT);

	switch (ev.type) {
		case JS_EVENT_BUTTON:
			if (ev.number < ps3dat->nr_buttons) {
				ps3dat->button[ev.number] = ev.value;
			}
			break;
		case JS_EVENT_AXIS:
			if (ev.number < ps3dat->nr_sticks) {
				ps3dat->stick[ev.number] = ev.value / 200; // 327 range -32767 ~ +32768 -> -100 ~ +100
			}
			break;
		default:
			break;
	}

	return 0;
}


int ps3c_getinfo(struct ps3ctls *ps3dat) {

	if(ioctl(ps3dat->fd , JSIOCGBUTTONS , &ps3dat->nr_buttons) < 0) return -1;
	if(ioctl(ps3dat->fd , JSIOCGAXES    , &ps3dat->nr_sticks ) < 0) return -2;

	return 0;
}


int ps3c_init(struct ps3ctls *ps3dat, const char *df) {

	unsigned char nr_btn;
	unsigned char nr_stk;
	unsigned char *p;
	int i;

	ps3dat->fd = open(df, O_RDONLY);
	if (ps3dat->fd < 0) return -1;

	if (ps3c_getinfo(ps3dat) < 0) {
		close(ps3dat->fd);
		return -2;
	}

	nr_btn = ps3dat->nr_buttons;
	nr_stk = ps3dat->nr_sticks;
//	printf("%d %d\n",nr_btn,nr_stk);

	p = calloc(nr_btn + nr_stk , sizeof(short));
	if (p == NULL) {
		close(ps3dat->fd);
		return -3;
	}
	ps3dat->button = (short *)p;
	ps3dat->stick  = (short *)&p[nr_btn * sizeof(short)];
	
//	for (i = 0; i<nr_btn; i++) ps3dat->button[i] = 0;
//	for (i = 0; i<nr_stk; i++) ps3dat->stick [i] = 0;
//	ps3dat->button[PAD_KEY_LEFT]=0;
//	ps3dat->button[PAD_KEY_RIGHT]=0;
//	ps3dat->button[PAD_KEY_UP]=0;;
//	ps3dat->button[PAD_KEY_DOWN]=0;
//	ps3dat->stick [PAD_LEFT_X]=0;
//	ps3dat->stick [PAD_LEFT_Y]=0;
//	ps3dat->stick [PAD_RIGHT_X]=0;
//	ps3dat->stick [PAD_RIGHT_Y]=0;

	return 0;
}

void ps3c_exit   (struct ps3ctls *ps3dat) {

	free (ps3dat->button);
	close(ps3dat->fd);
}


void main() {

	char *df = "/dev/input/js0";
	struct ps3ctls ps3dat;

	wiringPiSetup();
	softPwmCreate(5,0,20); // motor-1 10ms
	softPwmCreate(6,0,20); // motor-1 10ms
	softPwmCreate(26,0,20); // motor-2 10ms
	softPwmCreate(27,0,20); // motor-2 10ms
	softPwmCreate(28,0,20); // motor-3 10ms
	softPwmCreate(29,0,20); // motor-3 10ms
	softPwmCreate(24,0,20); // motor-4 10ms
	softPwmCreate(25,0,20); // motor-4 10ms
	softPwmCreate(14,0,20); // motor-5 10ms // NC
	softPwmCreate(23,0,20); // motor-5 10ms // NC
	softPwmCreate(3,0,20); // beep

	fds = wiringPiI2CSetup(0x40);	// PCA9685
	resetPCA9685(fds);
	setPCA9685Freq(fds,50);
	system("mpg123 /home/pi/Music/move_it.mp3 &");

	if(!(ps3c_init(&ps3dat, df))) {

		do {
			if (ps3c_test(&ps3dat) < 0) break;
		} while (!(ps3c_input(&ps3dat)));
		
		ps3c_exit(&ps3dat);		
	}
}

