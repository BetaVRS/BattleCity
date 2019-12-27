#include "TimeClock.h"

TimeClock::TimeClock()
{
	QueryPerformanceFrequency(&litmp);
	clkFreq = (double)litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;
}

void TimeClock::Init()
{
	QueryPerformanceCounter(&litmp);
	QPart1 = QPart2 = litmp.QuadPart;
}

void TimeClock::SetDeltaT(double t)
{
	deltaT = t;
}

bool TimeClock::IsTimeOut()
{
	QueryPerformanceCounter(&litmp);
	QPart2 = litmp.QuadPart;

	if ((double)((QPart2 - QPart1) * 1000) / clkFreq > deltaT)
	{
		QPart1 = QPart2;
		return true;
	}
	return false;
}

