#pragma once
#include "stdafx.h"

class TimeClock
{
public:
	TimeClock();
	~TimeClock(){}
	void Init();
	void SetDeltaT(double t);
	bool IsTimeOut();

private:
	LARGE_INTEGER litmp;
	LONGLONG QPart1, QPart2;
	double clkFreq;
	double deltaT;
};