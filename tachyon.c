//requires wiringop-zero https://github.com/xpertsavenue/WiringOP-Zero + patch it for ads1115 from mainline
//compile example:  gcc -Wall -o keyboard keyboard.c -lwiringPi -ludev
//then add it to your /etc/rc.local
//also don't forget udev rule below
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <fcntl.h>

#include <string.h>
#include <sys/ioctl.h>
#include <linux/uinput.h>
#include <libudev.h>

#include <wiringPi.h>
#include <ads1115.h>
#define MY_BASE 2222

//MAP PIN NAMES to wiringPI
#define PA12 8
#define PA11 9
#define PA06 7
#define PG06 15
#define PG07 16
#define PA01 0
#define PA07 1

#define NUM_OF_INPUTS 4
#define NUM_OF_OUTPUTS 3

#define DEADZONE_X 20
#define DEADZONE_Y 20

#define BUTTONS_COUNT NUM_OF_INPUTS*NUM_OF_OUTPUTS

//BUTTON MATRIX:
///////|  PG07 | PA01 | PA07
//PA12 | UP    |  A   | START
//PA11 | DOWN  |  Y   | SELECT
//PA06 | RIGHT |  B   | LL
//PG06 | LEFT  |  X   | LR

enum Buttons {
	MBTN_UP = 0,
	MBTN_DOWN = 1,
	MBTN_RIGHT = 2,
	MBTN_LEFT = 3,
	MBTN_A = 4,
	MBTN_Y = 5,
	MBTN_B = 6,
	MBTN_X = 7,
	MBTN_START = 8,
	MBTN_SELECT = 9,
	MBTN_LL = 10,
	MBTN_LR = 11
};

static uint32_t map_keyboard[BUTTONS_COUNT] = {
	BTN_TRIGGER, //uP
	BTN_THUMB, //DOWN
	BTN_THUMB2, //RIGHT
	BTN_TOP, //LEFT
	BTN_BASE,  //A
	BTN_BASE2,  //Y
	BTN_PINKIE,  //B
	BTN_BASE3,  //X
	BTN_TOP2,  //START
	BTN_BASE4,  //SELECT
	BTN_BASE5,  // LL
	BTN_BASE6   // LR

};

//for debuging only
static const char *btnstrings[] = {
	"UP",
	"DOWN",
	"RIGHT",
	"LEFT",
	"A",
	"Y",
	"B",
	"X",
	"START",
	"SELECT",
	"LL",
	"LR"};


static uint32_t inputs[NUM_OF_INPUTS] = {PA12, PA11, PA06, PG06};
static uint32_t outputs[NUM_OF_OUTPUTS] = {PG07, PA01, PA07};

static uint8_t matrix_status[NUM_OF_INPUTS*NUM_OF_OUTPUTS];

static int fdui,fdui2; // file handle for uinput

void printMatrix() {
	int i;
	for(i=0; i< BUTTONS_COUNT; i++)
	{
		if(matrix_status[i] == 1) {
			printf("BTN: %s is pressed\n\r",btnstrings[i] );
		}
	}
}

void processKey(int internal_key_id, uint8_t status) {
	int rc;
	struct input_event ie;
	//check if button status changed

	if(matrix_status[internal_key_id] != status)
	{
		memset(&ie, 0, sizeof(ie));
		if (status==0) // if state is off, send release
			ie.value = 0;
		else
			ie.value = 1; // send press

		ie.type = EV_KEY; // key event
		ie.code = map_keyboard[internal_key_id];
		rc = write(fdui, &ie, sizeof(ie)); // send the event
		ie.type = EV_SYN;
		ie.code = SYN_REPORT;
		ie.value = 0;
		rc = write(fdui, &ie, sizeof(ie)); // send a report
		if (rc < 0) {}; // suppress compiler warning
	}
}

void readMatrix() {
	int i,j;
	uint8_t status;
	for(i = 0; i<NUM_OF_OUTPUTS; i++)
	{
		//enable output line
		pinMode(outputs[i], OUTPUT);
		digitalWrite(outputs[i], 0);
		//check all inputs on this line
		for (j = 0; j < NUM_OF_INPUTS ; j++)
		{
			pullUpDnControl (inputs[j], PUD_UP);
			status = (digitalRead(inputs[j]) == LOW);
			processKey(i*NUM_OF_INPUTS +j, status);
			matrix_status[i*NUM_OF_INPUTS +j] = status;
			pullUpDnControl (inputs[j], PUD_OFF);
		}
		pinMode(outputs[i], INPUT);
		pullUpDnControl(outputs[i], PUD_OFF) ;
	}
}


int  setupVirtualInput(void)
{
	struct udev* _udev;
	struct udev_device* udev_dev;
	const char* devnode;
	int i;
	struct uinput_user_dev uidev;
	char* devPath;
	char  buf[100];
	char  evName[100] = "";

	_udev = udev_new();
	if (!_udev) {
		fprintf(stderr, "Error creating udev\n");
		return 1;
	}

	udev_dev = udev_device_new_from_subsystem_sysname(_udev, "misc", "uinput");
	if (!udev_dev) {
		fprintf(stderr, "Error creating udev device\n");
		return 1;
	}

	devnode = udev_device_get_devnode(udev_dev);
	if (!devnode) {
		fprintf(stderr, "Couldn't get devnode from udev device\n");
		return 1;
	}

	devPath = (char*) malloc(strlen(devnode) + 1);
	strcpy(devPath, devnode);
	udev_device_unref(udev_dev);
	udev_unref(_udev);


	fdui = open(devPath, O_WRONLY | O_NONBLOCK);
	if (fdui < 0)
	{
		fprintf(stderr, "Error opening /dev/uinput device\n");
		return 1;
	}


	memset(&uidev, 0, sizeof(uidev));

	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Matrix GPIO keyboard");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;

	ioctl(fdui, UI_SET_EVBIT, EV_KEY);
	for (i=0; i<BUTTONS_COUNT; i++)
	{
		ioctl(fdui, UI_SET_KEYBIT, map_keyboard[i]); // enable each key we will use
	}

	ioctl(fdui, UI_SET_EVBIT, EV_ABS);
    ioctl(fdui, UI_SET_ABSBIT, ABS_X);
    uidev.absmax[ABS_X] = 21000;
    uidev.absmin[ABS_X] = 3700;
    uidev.absfuzz[ABS_X] = 10;
    uidev.absflat[ABS_X] = 4;
    ioctl(fdui, UI_SET_ABSBIT, ABS_Y);
    uidev.absmax[ABS_Y] = 21000;
    uidev.absmin[ABS_Y] = 3700;
    uidev.absfuzz[ABS_Y] = 10;
    uidev.absflat[ABS_Y] = 4;


	int size = sizeof(struct uinput_user_dev);
	int w = write(fdui, &uidev, size);

	if (w != sizeof(struct uinput_user_dev)) {
		fprintf(stderr,"could not write device info\n");
		return 1;
	}


	if (ioctl(fdui, UI_DEV_CREATE) < 0)
	{
		fprintf(stderr, "Error creating virtual keyboard device\n");
		return 1;
	}

	//find for SDL
	struct stat st;
	for(i=99; i>=0; i--) {
		sprintf(buf, "/dev/input/event%d", i);
		if(!stat(buf, &st)) break; // last valid device
	}
	strcpy(evName, (i >= 0) ? buf : "/dev/input/event0");
	fdui2 = open(evName, O_WRONLY | O_NONBLOCK);
	fdui  = (fdui2 >= 0) ? fdui2 : fdui;
	if(fdui2) printf("SDL2 init OK\n");
	return 0;
}

void processAnalog(int chX, int chY) {
	int rc;
	struct input_event ie;
	static int lX=0, lY=0;
	//check if button status changed
	memset(&ie, 0, sizeof(ie));
	ie.type = EV_ABS; // key event

	if(abs(chX-lX) > DEADZONE_X){
		ie.value = chX; // send press
		ie.code = ABS_X;
		rc = write(fdui, &ie, sizeof(ie)); // send the event
		lX = chX;
	}
	if(abs(chY-lY) > DEADZONE_Y){
		ie.value = chY; // send press
		ie.code = ABS_Y;
		rc = write(fdui, &ie, sizeof(ie)); // send the event
		lY= chY;
	}
	ie.type = EV_SYN;
	ie.code = SYN_REPORT;
	ie.value = 0;
	rc = write(fdui, &ie, sizeof(ie)); // send a report
	if (rc < 0) {}; // suppress compiler warning

}


int main (void)
{
	int i, ch0 , ch1;

	printf ("Tachyon PI Zero Matrix Button Test\n") ;

	wiringPiSetup ();
	ads1115Setup (MY_BASE,"/dev/i2c-1", 0x48) ;

	if (setupVirtualInput())
	{
		fprintf(stderr, "Error configuring Virtual Input device \n");
		return 0;
	}

 	//configure all inputs
	for (i = 0 ; i < NUM_OF_INPUTS ; i++)
	{
		pinMode(inputs[i], INPUT);
		pullUpDnControl(inputs[i], PUD_OFF);
	}

	//disable all outputs
	for (i = 0 ; i < NUM_OF_OUTPUTS ; i++)
	{
		pinMode(outputs[i], INPUT) ;
		pullUpDnControl(outputs[i], PUD_OFF) ;
	}


	for (;;)
	{
		readMatrix();
		ch0 = analogRead (MY_BASE + 0) ;
  		ch1 = analogRead (MY_BASE + 1) ;
  		processAnalog(ch0, ch1);
		//printMatrix();
		delay (3) ;
	}
}