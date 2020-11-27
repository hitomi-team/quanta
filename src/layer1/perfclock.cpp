#include "pch/pch.h"

#include "perfclock.h"

void CPerfClock::Begin()
{
	this->time0 = SDL_GetPerformanceCounter();
}

void CPerfClock::End()
{
	this->time1 = SDL_GetPerformanceCounter();
	this->delta = static_cast< double >((this->time1 - this->time0))/static_cast< double >(SDL_GetPerformanceFrequency());
	this->deltaf32 = static_cast< float >(this->delta);
}
