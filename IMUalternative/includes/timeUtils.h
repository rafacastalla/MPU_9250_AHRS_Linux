#include <unistd.h>
#include <stdint.h>

#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

void delay(uint32_t us) {
	usleep((int) us);
}

long toMicroseconds(struct timespec *ts) {
	return ((ts->tv_sec) * 1000000 + (ts->tv_nsec) / 1000);
}

void sleep_until(struct timespec *ts, int delay) {
	long oneSecond = 1000 * 1000 * 1000;
	ts->tv_nsec += delay * 1000;
	if (ts->tv_nsec >= oneSecond) {
		ts->tv_nsec -= oneSecond;
		ts->tv_sec++;
	}
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ts, NULL);
}

void sleep_lapse(int delay) {
	long oneSecond = 1000 * 1000; //in microseconds
	struct timespec ts;
	ts.tv_sec = delay / oneSecond;
	ts.tv_nsec = (delay % oneSecond) * 1000;
	clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
}

long getCurrentMicroseconds() {
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	return (currentTime.tv_sec) * 1000000 + (currentTime.tv_nsec) / 1000;
}

#endif
