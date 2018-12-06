#ifndef TIMER_H_
#define TIMER_H_
#ifndef _WIN32
    #include <cstdlib>
//    #include <time.h>
#endif

class Timer {
	public:
		Timer();
		void start();
		double stop();
	private:
		#ifdef _WIN32
			double start_time;
			double freq;
		#else
			timeval _tstart, _tend;
		#endif
};

#endif /*TIMER_H_*/
