#ifndef LAYER1_PERFCLOCK_H
#define LAYER1_PERFCLOCK_H

struct CPerfClock {
	uint64_t time0, time1;

	// in seconds
	double delta;
	float deltaf32;

	inline CPerfClock() :
		time0(0),
		time1(0),
		delta(0.0),
		deltaf32(0.0f)
	{}

	void Begin();
	void End();
};

#endif
