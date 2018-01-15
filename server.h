#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <semaphore.h>
#include <ctype.h>

#define MOTOR1_ENABLE1 11
#define MOTOR1_ENABLE2 10
#define MOTOR2_ENABLE1 13
#define MOTOR2_ENABLE2 14
#define MOTOR1_PWM 2
#define MOTOR2_PWM 0
#define INFRARED 4

#define MAX_DISTANCE 20

#define F_TRIG 4
#define F_ECHO 5
#define R_TRIG 28
#define R_ECHO 27
#define L_TRIG 24
#define L_ECHO 23

#define GAS 6
#define FLAME 3
#define FAN 25
typedef struct 
{
    int front;
    int right;
    int left;
    int temperature;
    int gas;
    int flame;
    float dirDegree;
    float traveled;
 }ServerMessage;

typedef struct 
{
    int command;
    int drivingMode;
    int demoType;
    int motorState;
    int speed;
    
}ClientMessage;


//the thread function
void *connectionHandler(void *);
void *streaming_handler();

void* autoMode();
void manualMode(ClientMessage client_message, int moving);
void stopMotor();
void forward(int i);
void backward(int i);
void turnRight(int i);
void turnLeft(int i);
void zeroMotor();
void setup();
int getCM(int trig, int echo);
void* readTemp(void* dummy);
void* movement(void* dummy);
void turnAround();
void* readAllSensors(void*);
int readGas();
int readFlame();
void fanOn(int h);
void readAll();
float direction();
