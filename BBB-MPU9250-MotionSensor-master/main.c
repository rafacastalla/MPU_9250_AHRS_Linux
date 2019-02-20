#include <unistd.h>
#include <stdio.h>
#include <math.h>


#include <signal.h>

#include "timeUtils.h"

#include "MotionSensor.h"

int done = 0;
void register_sig_handler();
void sigint_handler(int sig);

int main() {

	int N = 10000;

	float angles[N][3];
	float mag[N][3];
	long times[N][2];

	//Register SIGINT handler for managed program termination
	register_sig_handler();

	printf("Start Collecting Data!\n");
	FILE *fptr;
	fptr = fopen("angles.csv", "w");
	if (fptr != NULL) {

	    int i = 0;

	    struct timespec ts;
	    clock_gettime(CLOCK_MONOTONIC, &ts);
	ms_open();
	while (!done && i < N){

		times[i][0] = getCurrentMicroseconds();

		ms_update();
		//printf("yaw = %2.1f\tpitch = %2.1f\troll = %2.1f\ttemperature = %2.1f\tcompass = %2.1f, %2.1f, %2.1f\n",ypr[YAW], ypr[PITCH],ypr[ROLL],temp,compass[0],compass[1],compass[2]);
		
		angles[i][0]=ypr[ROLL];
		angles[i][1]=ypr[PITCH];
		angles[i][2]=ypr[YAW];

		mag[i][0]=compass[ROLL];
		mag[i][1]=compass[PITCH];
		mag[i][2]=compass[YAW];

		i++;

		times[i][1] = getCurrentMicroseconds();

		sleep_until(&ts, 10000);
		
	}

		N = i;
		for(i = 0; i < N; i++){
			fprintf(fptr, "%f,%f,%f", angles[i][0], angles[i][1], angles[i][2]);
			fprintf(fptr, "%f,%f,%f", mag[i][0], mag[i][1], mag[i][2]);
			fprintf(fptr, ",%ld,%ld\r\n", times[i][0], times[i][1]);
		}

		fclose(fptr); //fptr is the file pointer associated with file to be closed.
		printf("Finish Collecting Data!\n");
		printf("Data stored in angles.csv\n");
	} else {
		printf("Error open file!");
		return -1;
	}

	return 0;
}

void register_sig_handler() {
	signal(SIGINT, sigint_handler);
}

void sigint_handler(int sig) {
	done = 1;
}

