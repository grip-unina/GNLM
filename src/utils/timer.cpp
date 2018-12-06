#include "timer.h"

#ifdef _WIN32
#include <windows.h>
Timer::Timer() {
	LARGE_INTEGER freq_li;
	QueryPerformanceFrequency(&freq_li);
	freq = (double)freq_li.QuadPart;
	start();
}
void Timer::start() {
	LARGE_INTEGER start_li;
	QueryPerformanceCounter(&start_li);
	start_time = (double)start_li.QuadPart;
}
double Timer::stop() {
	LARGE_INTEGER stop_li;
	QueryPerformanceCounter(&stop_li);
	double stop_time = (double)stop_li.QuadPart;
	return ((stop_time-start_time)/freq);
}
#else
Timer::Timer() {
	start();
}
void Timer::start() {
    gettimeofday(&_tstart, &tz);
}
double Timer::stop() {
    gettimeofday(&_tend,&tz);
    register double t1, t2;
    t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
    t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
    return t2-t1;
}
#endif

double time_info_0 = 0;
double time_info_1 = 0;
double time_info_2 = 0;
double time_info_3 = 0;
double time_info_4 = 0;
double time_info_5 = 0;
double time_info_6 = 0;
double time_info_7 = 0;
double time_info_8 = 0;
double time_info_9 = 0;

