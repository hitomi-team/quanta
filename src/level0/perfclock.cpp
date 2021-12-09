#include "level0/pch.h"

#include "perfclock.h"

PerfClock::PerfClock()
{
	this->beg = SDL_GetPerformanceCounter();
	this->end = this->beg; 
	this->dt_f64 = 0.;
	this->dt_f32 = 0.;
}

void PerfClock::Calc()
{
	this->end = this->beg;
	this->beg = SDL_GetPerformanceCounter();
	this->dt_f64 = ((this->beg - this->end)/static_cast< double >(SDL_GetPerformanceFrequency()));
	this->dt_f32 = static_cast< float >(this->dt_f64);
}
