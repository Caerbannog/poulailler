#include "Sunrise.h"

#include <stdlib.h>
#include <ctime>
#include <iostream>


#define LATTITUDE    43.6053    // Toulouse
#define LONGITUDE    1.4552052
#define TIMEZONE     +2

using namespace std;

int main(int argc, char **argv)
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	int month = 1 + ltm->tm_mon;
	int day = ltm->tm_mday;

	if (argc == 3) {
		day   = atoi(argv[1]);
		month = atoi(argv[2]);
	}

	cout << "Day     = " <<  day  << endl;
	cout << "Month   = " << month << endl;

	Sunrise mySunrise(LATTITUDE, LONGITUDE, TIMEZONE);
	mySunrise.Civil();

	int h, m;
	mySunrise.Rise(month, day);
	h = mySunrise.Hour();
	m = mySunrise.Minute();
	cout << "Sunrise = " << h << ":" << m << endl;

	mySunrise.Set(month, day);
	h = mySunrise.Hour();
	m = mySunrise.Minute();
	cout << "Sunset  = " << h << ":" << m << endl;

	return 0;
}
