#ifndef LEVEL0_PERFCLOCK_H
#define LEVEL0_PERFCLOCK_H

struct PerfClock {
	uint64_t beg, end;
	double dt_f64;
	float dt_f32;

	PerfClock();
	void Calc();
};

#endif
