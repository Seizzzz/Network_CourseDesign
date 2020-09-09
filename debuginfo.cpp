#include "debuginfo.h"

int infoLevel = 0;
int number = 0;

time_t getMilliSec()
{
	return clock();
}

void debugInfo(int level, const char* format, ...)
{
	//int infoLevel = 0;
	if (level <= infoLevel)
	{
		printf("\t%.4lf:\t%d:\t", (double)getMilliSec() / CLOCKS_PER_SEC, number++);

		va_list args;

		va_start(args, format);
		vprintf(format, args);

		va_end(args);
	}
}