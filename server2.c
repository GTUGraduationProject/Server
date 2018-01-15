#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "server.h"


static int connFd;


ClientMessage clientMessage;
ServerMessage serverMessage;

pthread_t sensorThread, autoModeThread;
pthread_mutex_t sensorMutex;
pthread_t flameThread, temperatureThread;
pthread_t frTh, lfTh, rgTh, stTh, bckTh, zrTh;
pthread_mutex_t flameMutex;
pthread_t denemeThread;

int denemevalue = 0;
int speed = 15;
int magneFd;
pthread_t threadIDs[1000];
int threadCounter = 0;
int main(int argc, char* argv[])
{

    setup();
    pthread_mutex_init(&sensorMutex, NULL);
    pthread_mutex_init(&flameMutex, NULL);
    int pId, portNo, listenFd;
    socklen_t len; //store size of the address
    bool loop = false;
    struct sockaddr_in svrAdd, clntAdd;
    
    pthread_t threadA[3];
    
    if (argc < 2)
    {
		fprintf(stderr,"Syntam : ./server <port>" );
        return 0;
    }
    fprintf(stderr,"%lf", 5.0*cos(160.0*3.14/180.0));
    portNo = atoi(argv[1]);
    
    if((portNo > 65535) || (portNo < 2000))
    {
		fprintf(stderr,"Please enter a port number between 2000 - 65535" );
        return 0;
    }
    
    //create socket
    listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(listenFd < 0)
    {
		fprintf(stderr,"Cannot open socket" );

        return 0;
    }
    
    bzero((char*) &svrAdd, sizeof(svrAdd));
    
    svrAdd.sin_family = AF_INET;
    svrAdd.sin_addr.s_addr = INADDR_ANY;
    svrAdd.sin_port = htons(portNo);
    
    //bind socket
    if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
    {
        fprintf(stderr,"can not bind" );
        return 0;
    }
    
    listen(listenFd, 5);
    
    int noThread = 0;

    while (noThread < 3)
    {	
        socklen_t len = sizeof(clntAdd);
/*		for(int i=0 ; i<22 ; ++i)
			turnRight(20);*/
        fprintf(stderr,"Listening" );

        connFd = accept(listenFd, (struct sockaddr *)&clntAdd, &len);

        if (connFd < 0)
        {
            fprintf(stderr,"Cannot accept connection" );
            return 0;
        }
        else
        {
            fprintf(stderr,"Connection successful");
        }
        char* ip = argv[1];
        int childpid;
		if ((childpid = fork()) == -1) {
			perror("Failed to create child process");
			exit(1) ;
		}
		if(childpid==0){ /*child*/
			streaming_handler((void *)ip);

			exit(1);
		}	
		else{ 
			connectionHandler(NULL);
		}
        noThread++;
    }
    
/*    for(int i = 0; i < 3; i++)
    {
        pthread_join(threadA[i], NULL);
    }*/
    
    
}


void *connectionHandler (void *dummyPt)
{
	int childpid;
    fprintf(stderr,"Thread No: %d",pthread_self());
    char test[300];
    bzero(test, 301);
    bool loop = false;
    if ((childpid = fork()) == -1) {
		perror("Failed to create child process");
		exit(1) ;
	}
	if(childpid==0){ /*child*/
		system("cd ; cd  Desktop/bitirme/voice/src/darkice-1.0/ ; sudo darkice -c darkice.cfg");
		exit(1);
	}	
	else{ /*parent*/

		while(read(connFd, &clientMessage, sizeof(ClientMessage)))
		{    

			if(clientMessage.drivingMode == 1){
				fprintf(stderr,"automode" ); 

				autoMode(NULL);
			}
			else if(clientMessage.drivingMode == 2){
				fprintf(stderr,"manuelmode" ); 
				//read(connFd, &clientMessage, sizeof(ClientMessage));
				manualMode(clientMessage, 0);
			}
		}
		fprintf(stderr,"\nClosing thread and conn" );
		close(connFd);
	}
}

void* streaming_handler(void* ip){
	puts((char*)ip);
	char call[255] = "raspivid -n -t 0 -rot 0 -w 640 -h 480 -fps 25 -b 6000000 -o - | gst-launch-1.0 -e -vvvv fdsrc ! h264parse ! rtph264pay pt=96 config-interval=5 ! udpsink host=10.1.18.4 port=5000";
	//strcat(call, (char*)ip);
	//strcat(call, " port=5000");	
	system(call);	
	exit(1);
}

void* autoMode(void* dummy){
	
    fprintf(stderr,"Thread No: " );

    int on, sag, sol;
    int moving = 0;
    int f = 0;
    int degree=0;
    long int xd=0, yd=0, zd=0;
    double angle, mag;
    int turnFlag = 1;
    float frontCons=0.0, leftCons=0.0, rightCons=0.0;
    serverMessage.front = getCM(F_TRIG, F_ECHO);
    serverMessage.left = getCM(L_TRIG, L_ECHO);
    serverMessage.right = getCM(R_TRIG, R_ECHO);
    serverMessage.dirDegree = 90.0;
    speed = 50;
    fprintf(stderr,"stopsdfafbgfdwsfsf" );
    while(clientMessage.motorState != 0){
		//direction();
		//fprintf(stderr, "%d \n", serverMessage.right);
    	if(clientMessage.motorState == 0){
            fprintf(stderr,"stopsdfafbgfdwsfsf" );
    		stopMotor();
			zeroMotor();
    		break;
    	}
    	if(clientMessage.speed == 10){
			frontCons = 0.8;
			leftCons = 2.0;
			rightCons = 1.83;
		}
		else if(clientMessage.speed == 20){
			frontCons = 2.8;
			leftCons = 8.65;
			rightCons = 8.181;	
		}
		else if(clientMessage.speed == 30){
			frontCons = 10.0;
			leftCons = 11.25;
			rightCons = 11.25;	
		}
		else if(clientMessage.speed == 40){
			frontCons = 5.0;
			leftCons = 12.857;
			rightCons = 12.857;	
		}
		else if(clientMessage.speed == 50){
			frontCons = 5.5;
			leftCons = 13.846;
			rightCons = 14.285;	
		}
		readAllSensors(NULL);
		//puts("general\n");

		f = serverMessage.flame;
        on = serverMessage.front;
        sag = serverMessage.right;
        sol = serverMessage.left;
		fprintf(stderr, "%d %lf ", serverMessage.temperature, serverMessage.dirDegree);
        if(0){
        	fanOn(1);
    		stopMotor();
    		zeroMotor();
        }
        else{
        	fanOn(0);
			if(on >= MAX_DISTANCE +4 ){
				puts("forward\n");
				forward(speed);
				serverMessage.traveled = frontCons;
				//readAllSensors(NULL);


			}
			else if(turnFlag==1 && on < MAX_DISTANCE+4 && sag >= MAX_DISTANCE + 15 ){

/*				readAllSensors(NULL);
				f = serverMessage.flame;
				on = serverMessage.front;
				sag = serverMessage.right;
				sol = serverMessage.left;
				if(on < MAX_DISTANCE && sag >= MAX_DISTANCE){*/
				puts("right\n");
					while(serverMessage.front < MAX_DISTANCE +10){
						//backward(speed);
						//serverMessage.traveled = -frontCons;
						turnRight(speed);
						serverMessage.dirDegree -= rightCons;
						readAllSensors(NULL);
						
					}
					turnRight(speed);
					serverMessage.dirDegree -= rightCons;
					turnRight(speed);
					serverMessage.dirDegree -= rightCons;
					stopMotor();
					turnFlag = 0;
					
				//}
			}

			else if(turnFlag == 0 && on < MAX_DISTANCE+4 && sol >= MAX_DISTANCE + 15){
				/*readAllSensors(NULL);
				f = serverMessage.flame;
				on = serverMessage.front;
				sag = serverMessage.right;
				sol = serverMessage.left;
				if(on < MAX_DISTANCE && sol >= MAX_DISTANCE){*/
					puts("left\n");
					//readAllSensors(NULL);
					while(serverMessage.front < MAX_DISTANCE + 10){
						//backward(speed);
						//serverMessage.traveled = -frontCons;
						turnLeft(speed);
						serverMessage.dirDegree += leftCons;
						readAllSensors(NULL);
					}
					turnLeft(speed);
					serverMessage.dirDegree += leftCons;
					turnLeft(speed);
					serverMessage.dirDegree += leftCons;
					stopMotor();
					turnFlag = 1;
					
				//}
			}
			else if(on < MAX_DISTANCE && sag < MAX_DISTANCE && sol < MAX_DISTANCE){

				puts("backward\n");
				while(1){
					readAllSensors(NULL);
					f = serverMessage.flame;
					on = serverMessage.front;
					sag = serverMessage.right;
					sol = serverMessage.left;				
					backward(speed);
					serverMessage.traveled = -frontCons;
					if(sag >= MAX_DISTANCE || sol >= MAX_DISTANCE)
						break;
					
				}	
				if(sag >= MAX_DISTANCE){
/*					readAllSensors(NULL);
					f = serverMessage.flame;
					on = serverMessage.front;
					sag = serverMessage.right;
					sol = serverMessage.left;
					if(sag >= MAX_DISTANCE){*/
						puts("backright\n");
						backward(speed);
						serverMessage.traveled = -frontCons;
						delay(200);
						stopMotor();

						delay(25);
						turnRight(speed);
						serverMessage.dirDegree -= rightCons;
						delay(400);
						stopMotor();

						delay(75);
						
					//}
				}
				else if(sol >= MAX_DISTANCE){
					/*readAllSensors(NULL);
					f = serverMessage.flame;
					on = serverMessage.front;
					sag = serverMessage.right;
					sol = serverMessage.left;
					if( sol >= MAX_DISTANCE){*/
						puts("backleft\n");
						backward(speed);
						serverMessage.traveled = -frontCons;
						delay(200);
						stopMotor();

						delay(25);
						turnLeft(speed);
						serverMessage.dirDegree += leftCons;
						delay(400);
						turnLeft(speed);
						serverMessage.dirDegree += leftCons;
						delay(75);
						
					//}
				}
			}
			else{
				if(turnFlag)
					turnFlag = 0;
				else
					turnFlag = 1;
			}
		}

		//printf("----\n");
		if(serverMessage.dirDegree > 360.0)
			serverMessage.dirDegree -= 360.0;
		else if(serverMessage.dirDegree < 0.0)
			serverMessage.dirDegree += 360.0;
		
		readAllSensors(NULL);
		if(serverMessage.right < MAX_DISTANCE  && serverMessage.right < sag){
			turnLeft(speed);
			serverMessage.dirDegree += leftCons;
		}		
		if(serverMessage.left < MAX_DISTANCE  && serverMessage.left < sol){
			turnRight(speed);
			serverMessage.dirDegree -= rightCons;
		}

    }
    close(connFd);
}


void* flame(void* dummy){
	serverMessage.flame = readFlame();
}

void manualMode(ClientMessage client_message, int moving){
	
	
//	pthread_join(flameThread, NULL);
    float frontCons=0.0, leftCons=0.0, rightCons=0.0;
    serverMessage.dirDegree = 90.0;
    speed = 50;
	while(1){
		if(clientMessage.speed == 10){
			frontCons = 0.8;
			leftCons = 2;
			rightCons = 1.83;
		}
		else if(clientMessage.speed == 20){
			frontCons = 2.8;
			leftCons = 2;
			rightCons = 1.83;	
		}
		else if(clientMessage.speed == 30){
			frontCons = 4.2;
			leftCons = 5.62;
			rightCons = 5;	
		}
		else if(clientMessage.speed == 40){
			frontCons = 5;
			leftCons = 8.18;
			rightCons = 7.5;	
		}
		else if(clientMessage.speed == 20){
			frontCons = 5.5;
			leftCons = 11.25;
			rightCons = 11.25;	
		}
		readAllSensors(NULL);
	//	pthread_mutex_lock(&flameMutex);
	//	int f = serverMessage.flame;
	//	pthread_mutex_unlock(&flameMutex);
		
		if(serverMessage.flame == 1){
			//printf("fan: \n", clientMessage.fan);
			fanOn(0);
		}
		else if(serverMessage.flame == 0){
			//printf("fan: \n", clientMessage.fan);
			fanOn(1);
		}
		
		//fprintf(stderr, "%d\n", clientMessage.command);
		if(clientMessage.command == 5){
			if(moving != 5)
				stopMotor();
			moving = 5;
			zeroMotor();
			serverMessage.traveled = 0;
		}
		else if(clientMessage.command == 1){
			//printf("ileri\n");
			//cout << "fan: " << client_message.fan << endl;
			if(moving != 1)
				stopMotor();
			moving = 1;
			forward(speed);
			serverMessage.traveled = frontCons;
			//delay(200);
			//stopMotor();
			}
		else if(clientMessage.command == 2){
			if(moving != 2)
				stopMotor();
			moving = 2;
			backward(speed);
			serverMessage.traveled = -frontCons;
			//delay(200);
			//stopMotor();
		}
		else if(clientMessage.command == 3){
			//sol
			if(moving != 3)
				stopMotor();
			moving = 3;
			turnLeft(speed);
			serverMessage.dirDegree += leftCons; 
			//delay(250);
			//stopMotor();
		}
		else if(clientMessage.command == 4){
			//sag
			if(moving != 4)
				stopMotor();
			moving = 4;
			turnRight(speed);
			serverMessage.dirDegree -= rightCons;
			//delay(250);
			//stopMotor();
		}

		else {
			if(moving != 0)
				stopMotor();
			moving = 0;
			zeroMotor();
			serverMessage.traveled = 0;
		}
	}

}

void *readAllSensors(void* dummy){

    serverMessage.front = getCM(F_TRIG, F_ECHO);
    serverMessage.left = getCM(L_TRIG, L_ECHO);
    serverMessage.right = getCM(R_TRIG, R_ECHO);
    
	pthread_create(&(threadIDs[threadCounter++]), NULL, readTemp, NULL);
	pthread_create(&(threadIDs[threadCounter++]), NULL, flame, NULL);
    serverMessage.gas = readGas();	
	write(connFd, &serverMessage, sizeof(ServerMessage));
	read(connFd, &clientMessage, sizeof(ClientMessage));
	speed = clientMessage.speed;
}

int getCM(int trig, int echo) {
        //Send trig pulse
	digitalWrite(trig, HIGH);
	delayMicroseconds(10);
	digitalWrite(trig, LOW);
    //fprintf(stderr, "sensoronce\n");

	long echoTimer = micros();
	//Wait for echo start
	while(digitalRead(echo) == LOW){
		if((micros() - echoTimer) > 10000)
			return 25;
	}
    //fprintf(stderr, "sensorsonra\n");
	//Wait for echo end
	long startTime = micros();
	while(digitalRead(echo) == HIGH);
    //fprintf(stderr, "sensorbitis\n");
	long travelTime = micros() - startTime;
	int distance = travelTime / 58.0;
	if(distance > 100){
		return 100;
	}
        return distance;
}

int readGas(){
	if(digitalRead(GAS) == 0){
		return 1;
	}
	else if(digitalRead(GAS) == 1){
		return 0;
	}

}

int readFlame(){

	if(digitalRead(FLAME) == 1){
		return 1;
	}
	else if(digitalRead(FLAME) == 0){
		return 0;
	}
}

void* readTemp(void* dummy)
{
	FILE *device = fopen("/sys/bus/w1/devices/28-0316848984ff/w1_slave", "r");
	double temperature = -1;
	char crcVar[5];
	if (device == NULL)
	{
		printf("Check connections \n");
		perror("\n");
	}
	if (device != NULL)
	{
		if (!ferror(device))
		{
			fscanf(device, "%*x %*x %*x %*x %*x %*x %*x %*x %*x : crc=%*x %s", crcVar);
			if (strncmp(crcVar, "YES", 3) == 0)
			{
				fscanf(device, "%*x %*x %*x %*x %*x %*x %*x %*x %*x t=%lf", &temperature);
				//printf("%.3lf\n",temperature);
				temperature /= 1000.0;
			}
		}
	}
	fclose(device);
	serverMessage.temperature = temperature;
}

void setup() {
	magneFd = wiringPiI2CSetup(0x0d);	// QMC5883L
	wiringPiI2CWriteReg8(magneFd,0x0b,0x01);	
	wiringPiI2CWriteReg8(magneFd,0x09,0x1d);
	
    wiringPiSetup();

    pinMode(MOTOR1_ENABLE1, OUTPUT); // in1
    pinMode(MOTOR1_ENABLE2, OUTPUT); // in2
    pinMode(MOTOR2_ENABLE1, OUTPUT); // in3
    pinMode(MOTOR2_ENABLE2, OUTPUT); // in4
	pinMode(MOTOR1_PWM, PWM_OUTPUT);
	pinMode(MOTOR2_PWM, PWM_OUTPUT);
    pinMode(FAN, OUTPUT);
    
	pinMode(F_TRIG, OUTPUT);
    pinMode(F_ECHO, INPUT);
    pinMode(R_TRIG, OUTPUT);
    pinMode(R_ECHO, INPUT);
    pinMode(L_TRIG, OUTPUT);
    pinMode(L_ECHO, INPUT);
	digitalWrite(F_TRIG, LOW);
    digitalWrite(R_TRIG, LOW);
    digitalWrite(L_TRIG, LOW);
    pinMode(GAS, INPUT); // in1
    pinMode(FLAME, INPUT);
        //TRIG pin must start LOW
	if(softPwmCreate(MOTOR1_PWM, 0, 50)!=0 ||
		softPwmCreate(MOTOR2_PWM, 0, 50)!=0)
		exit(1);
	softPwmWrite(MOTOR1_PWM,20);
	softPwmWrite(MOTOR2_PWM,20);

    digitalWrite(GAS, LOW);
    digitalWrite(FLAME, LOW);
    digitalWrite(FAN, LOW);
    stopMotor();
    zeroMotor();
    delay(30);
}

void backward(int i){
	softPwmWrite(MOTOR1_PWM,i);
	softPwmWrite(MOTOR2_PWM,i);
	digitalWrite(MOTOR1_ENABLE1,HIGH);
	digitalWrite(MOTOR1_ENABLE2,LOW);
	digitalWrite(MOTOR2_ENABLE1,LOW);
	digitalWrite(MOTOR2_ENABLE2,HIGH);
	delay(100);
	softPwmWrite(MOTOR1_PWM,0);
	softPwmWrite(MOTOR2_PWM,0);
}

void forward(int i){

	softPwmWrite(MOTOR1_PWM,i);
	softPwmWrite(MOTOR2_PWM,i);
	digitalWrite(MOTOR1_ENABLE1,LOW);
	digitalWrite(MOTOR1_ENABLE2,HIGH);
	digitalWrite(MOTOR2_ENABLE1,HIGH);
	digitalWrite(MOTOR2_ENABLE2,LOW);
	delay(100);
	softPwmWrite(MOTOR1_PWM,0);
	softPwmWrite(MOTOR2_PWM,0);
	
}

void turnLeft(int i){
	softPwmWrite(MOTOR1_PWM,i);
	softPwmWrite(MOTOR2_PWM,i);
	digitalWrite(MOTOR1_ENABLE1,HIGH);
	digitalWrite(MOTOR1_ENABLE2,LOW);
	digitalWrite(MOTOR2_ENABLE1,HIGH);
	digitalWrite(MOTOR2_ENABLE2,LOW);
	delay(100);
	softPwmWrite(MOTOR1_PWM,0);
	softPwmWrite(MOTOR2_PWM,0);
}

void turnRight(int i){

	softPwmWrite(MOTOR1_PWM,i);
	softPwmWrite(MOTOR2_PWM,i);
	digitalWrite(MOTOR1_ENABLE1,LOW);
	digitalWrite(MOTOR1_ENABLE2,HIGH);
	digitalWrite(MOTOR2_ENABLE1,LOW);
	digitalWrite(MOTOR2_ENABLE2,HIGH);
	delay(100);
	softPwmWrite(MOTOR1_PWM,0);
	softPwmWrite(MOTOR2_PWM,0);	
}

void stopMotor(){
	
	digitalWrite(MOTOR1_ENABLE1, HIGH); 
    digitalWrite(MOTOR2_ENABLE1, HIGH);
	digitalWrite(MOTOR1_ENABLE2, HIGH);
	digitalWrite(MOTOR2_ENABLE2, HIGH);
	delay(250);
    zeroMotor();
}

void zeroMotor(){       
    digitalWrite(MOTOR1_ENABLE1, LOW); 
    digitalWrite(MOTOR2_ENABLE1, LOW);
	digitalWrite(MOTOR1_ENABLE2, LOW);
	digitalWrite(MOTOR2_ENABLE2, LOW);
}

void fanOn(int h){
    if(h == 1)
        digitalWrite(FAN, HIGH);
    else if(h == 0)
        digitalWrite(FAN, LOW);
}
float direction(){
	long int xd, yd, zd, x=0,y=0,z=0;
	float declinationAngle = (5.0 + (28.0 / 60.0)) / (180 / M_PI);
	for(int i=0 ; i<10 ;++i){
		xd = (wiringPiI2CReadReg8(magneFd,1)<<8) | wiringPiI2CReadReg8(magneFd,0);
		x+= xd;
		yd = (wiringPiI2CReadReg8(magneFd,3)<<8) | wiringPiI2CReadReg8(magneFd,2);
		y+= yd;
		zd =(wiringPiI2CReadReg8(magneFd,5)<<8) | wiringPiI2CReadReg8(magneFd,4);
		z+=zd;
	}
	x = x/10;
	y = y/10;
	z = z/10;
	if(y>32767)
		y -= 65536;
	if(x>32767)
		x -= 65536;
	if(z>32767)
		z -= 65536;
	float heading = atan2(y, x) + declinationAngle;
	if (heading < 0){
		heading += 2 * M_PI;
	}

	if (heading > 2 * M_PI){
		heading -= 2 * M_PI;
	}
	float headingDegrees = heading * 180/M_PI;
	//printf("%.2f %.2f\n", heading, headingDegrees);
	serverMessage.dirDegree = headingDegrees;
	return headingDegrees;
}
