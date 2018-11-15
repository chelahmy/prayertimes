// prayertimes.c
// Prayer Times Calculator
// Adapted from PrayTimes.js ver 2.3 by PrayTimes.org
// not a direct translation from the Javascript codes to C
// freed from the standard math or any library
// for the purpose to run it in embedded systems
// by Abdullah Daud, chelahmy@gmail.com
// started on 3 Disember 2015
// completed on 19 October 2018
// on as-is-time basis

#include "prayertimes.h"

// Notes:
// 1. The times of sunrise, noon, sunset, and midnight can be calculated
//     using standard scientific methods.
// 2. The times for fajr and isha depends on various world standards. They
//     use different angles against sunrise or sunset. Some uses a period
//     before or after sunrise or sunset.
// 3. Maghrib can be fixed to sunset. However, some standards delay it
//     based on angles of the Sun after sunset.
// 4. Asar is generally fixed to the time when the length of an object's
//     shadow is equal to the object's height plus the object's shadow
//     length at noon. However, the Hanafi school fixed it to double the
//     object's shadow against its height plus its shadow length at noon.
// 5. A prayer time can be adjusted to a certain angle or time of the Sun
//     relative to a known event such as sunrise, noon, sunset or other
//     prayer time. However, it may only be relevant to certain locations
//     due to the declination of the Sun throughout the year.
//     
//

//---------------------- Math routines --------------------

// IEEE 754 floating point NAN value
// https://stackoverflow.com/a/1923903
float p_nan(void)
{
	int nan = 0x7F800001;
	return *(float*)&nan;
}

// Absolute value
double p_abs(double v)
{
	return v < 0.0 ? -v : v;
}

// 10^pow for used by p_sqrt()
long p_10(short pow)
{
	long t = 1;
	for (; pow > 0; t *= 10, --pow);
	return t;
}

// Square root based on algorithm
// from https://www.homeschoolmath.net/teaching/square-root-algorithm.php
double p_sqrt(double v)
{
	long t, d, b = 0, ir = 0, s, p10, i = (long)v; // integer part
	double f = v - (double)i; // fragment part
	short j, k, c = 1;
	for (t = i; t /= 10; c++); // count digits
	
	// extract the first digits
	if (c % 2 > 0)
		--c;
	else
		c -= 2;

	// start with the integer part
	t = i;
	do
	{
		// car i
		p10 = p_10(c);
		d = (long)(t / p10);
		t -= d * p10;
		
		// append to balance
		b *= 100;
		b += d;
		
		s = ir * 20; // side note 
		
		for (j = 0; j < 10; j++)
		{
			if ((s + j) * j > b)
				break;
		}
		
		if (j > 0)
			--j;
		
		// add to the integer result
		ir *= 10;
		ir += j;
		
		b -= (s + j) * j; // balance to push forward
		
		c -= 2;
	}
	while (c >= 0);
	
	// continue with the fraction part
	for (k = 0; k < 8; k++)
	{
		f *= 100;
		d = (long)f;
		f -= d;
		
		// append to balance
		b *= 100;
		b += d;
		
		s = ir * 20; // side note 
		
		for (j = 0; j < 10; j++)
		{
			if ((s + j) * j > b)
				break;
		}
		
		if (j > 0)
			--j;
		
		// add to the integer result
		ir *= 10;
		ir += j;
		
		b -= (s + j) * j; // balance to push forward
	}
	
	return (double)ir / p_10(8);
}

//---------------------- Degree-based trignometry routines -----------------------

short p_reduce(double *x)
{
	short q = 0;
	
	while (*x > P_2PI)
		*x -= P_2PI;
		
	if (*x > P_hPI && *x <= P_PI)
	{
		*x -= P_hPI;
		*x = P_hPI - *x;
		q = 1;
	}
	else if (*x > P_PI && *x <= P_3hPI)
	{
		*x -= P_PI;
		q = 2;
	}
	else if (*x > P_3hPI)
	{
		*x -= P_3hPI;
		*x = P_hPI - *x;
		q = 3;
	}
	
	return q;
}

// The trignometry algorithms are based on the Taylor Series
// https://en.wikipedia.org/wiki/Taylor_series
// Shorter series improves speed but reduces accuracy.

double p_sin(double x)
{
	double rst, p;
	short q, minus = 0;

	q = p_reduce(&x);
	
	if (q >= 2)
		minus = 1;
	
	rst = x; // x
	p = x;
	p *= x;
	p *= x;
	rst -= (p / 6.0); // - (x^3 / 3!)
	p *= x;
	p *= x;
	rst += (p / 120.0); // + (x^5 / 5!)
	p *= x;
	p *= x;
	rst -= (p / 5040.0); // - (x^7 / 7!)
	p *= x;
	p *= x;
	rst += (p / 362880.0); // + (x^9 / 9!)
	p *= x;
	p *= x;
	rst -= (p / 39916800.0); // - (x^11 / 11!)
	p *= x;
	p *= x;
	rst += (p / 6227020800.0); // + (x^13 / 13!)
	p *= x;
	p *= x;
	rst -= (p / 1307674368000.0); // - (x^15 / 15!)

	return minus ? -rst : rst;
}

double p_cos(double x)
{
	double rst, p;
	short q, minus = 0;

	q = p_reduce(&x);
	
	if (q == 1 || q == 2)
		minus = 1;
	
	rst = 1.0; // 1
	p = x;
	p *= x;
	rst -= (p / 2.0); // - (x^2 / 2!)
	p *= x;
	p *= x;
	rst += (p / 24.0); // + (x^4 / 4!)
	p *= x;
	p *= x;
	rst -= (p / 720.0); // - (x^8 / 8!)
	p *= x;
	p *= x;
	rst += (p / 40320.0); // + (x^10 / 10!)
	p *= x;
	p *= x;
	rst -= (p / 3628800.0); // - (x^12 / 12!)	
	p *= x;
	p *= x;
	rst += (p / 87178291200.0); // + (x^14 / 14!)
	p *= x;
	p *= x;
	rst -= (p / 20922789888000.0); // - (x^16 / 16!)
	p *= x;
	p *= x;
	rst += (p / 6402373705728000.0); // + (x^18 / 18!)
	p *= x;
	p *= x;
	rst -= (p / 2432902008176640000.0); // - (x^20 / 20!)

	return minus ? -rst : rst;
}

double p_tan(double x)
{
	return p_sin(x) / p_cos(x);
}

double p_asin(double x)
{
	double rst, p;
	
	rst = x; // x
	p = x;
	p *= x;
	p *= x;
	rst += (p / 6.0); // + (x^3 / 6)
	p *= x;
	p *= x;
	rst += (3.0 * p / 40.0); // + (3x^5 / 40)
	p *= x;
	p *= x;
	rst += (5.0 * p / 112.0); // + (5x^7 / 112)
	p *= x;
	p *= x;
	rst += (35.0 * p / 1152.0); // + (35x^9 / 1152)

	return rst;
}

double p_acos(double x)
{
	return P_hPI - p_asin(x);
}
/*
double p_atan(double x)
{
	double rst, p;
	
	rst = x; // x
	p = x;
	p *= x;
	p *= x;
	rst -= (p / 3.0); // - (x^3 / 3)
	p *= x;
	p *= x;
	rst += (p / 5.0); // + (x^5 / 5)
	p *= x;
	p *= x;
	rst -= (p / 7.0); // - (x^7 / 7)
	p *= x;
	p *= x;
	rst += (p / 9.0); // + (x^9 / 9)	
	p *= x;
	p *= x;
	rst -= (p / 11.0); // - (x^11 / 11)
	p *= x;
	p *= x;
	rst += (p / 13.0); // + (x^13 / 13)
	p *= x;
	p *= x;
	rst -= (p / 15.0); // - (x^15 / 15)

	return rst;
}

// https://en.wikipedia.org/wiki/Atan2
double p_atan2(double y, double x)
{
	if (x == 0.0 && y == 0.0) // undefined
		return p_nan();

	if (x == 0.0)
	{
		if (y > 0)
			return P_hPI;
		else
			return -P_hPI;
	}
			
	if (x > 0.0)
		return p_atan(y / x);

	if (y >= 0.0)
		return p_atan(y / x) + P_PI;
		
	return p_atan(y / x) - P_PI;
}
*/
// degree to radian
double dm_dtr(double d)
{
	return (d * P_PI) / 180.0;
}

// radian to degree
double dm_rtd(double r)
{
	return (r * 180.0) / P_PI;
}

double dm_sin(double d)
{
	return p_sin(dm_dtr(d));
}

double dm_cos(double d)
{
	return p_cos(dm_dtr(d));
}

double dm_tan(double d)
{
	return p_tan(dm_dtr(d));
}

double dm_arcsin(double d)
{
	return dm_rtd(p_asin(d));
}

double dm_arccos(double d)
{
	return dm_rtd(p_acos(d));
}

double dm_arctan(double d)
{
	return dm_rtd(p_atan(d));
}

double dm_arccot(double x)
{
	return dm_rtd(p_atan(1.0/x));
}

double dm_arctan2(double y, double x)
{
	return dm_rtd(p_atan2(y, x));
}

double dm_fix(double a, double b)
{
	a = a - b * (long)(a / b);
	return a < 0.0 ? a + b : a;
}

double dm_fixAngle(double a)
{
	return dm_fix(a, 360.0);
}

double dm_fixHour(double a)
{
	return dm_fix(a, 24.0);
}

//---------------------- Solar-geometric functions -----------------------

// Compute the declination angle of the Sun and the equation of time
// for a given Julian date *jd* which includes the clock time of the day.
// Ref: http://aa.usno.navy.mil/faq/docs/SunApprox.php
// Ref: https://en.wikipedia.org/wiki/Position_of_the_Sun
// decl : declination angle of the sun, eqt : equation (correction) of time
void sunPosition(double jd, double *decl, double *eqt)
{
	double D, g, q, L, e, RA;
	
	D = jd - 2451545.0; // days since Greenwich noon, Terrestrial Time, on 1 January 2000 (j2000.0)

	g = dm_fixAngle(357.529 + 0.98560028 * D); // the mean anomaly of the Sun
	q = dm_fixAngle(280.459 + 0.98564736 * D); // the mean longitude of the Sun, corrected for the aberration of light
	L = dm_fixAngle(q + 1.915 * dm_sin(g) + 0.020 * dm_sin(2.0 * g)); // the ecliptic longitude of the Sun

	//double R = 1.00014 - 0.01671 * dm_cos(g) - 0.00014 * dm_cos(2 * g); // the distance of the Sun from the Earth, in astronomical units
	e = 23.439 - 0.00000036 * D; // the obliquity of the ecliptic (approx)
	RA = dm_arctan2(dm_cos(e) * dm_sin(L), dm_cos(L)) / 15.0; // right ascension time
	
	*decl = dm_arcsin(dm_sin(e) * dm_sin(L)); // declination of the Sun
	*eqt = q / 15.0 - dm_fixHour(RA); // equation of time
}

// Compute the time different against mid-day at a *lat*itude position when
// the Sun is at an *angle* below the horizon and at a specific *decl*ination
// angle of the year.
// Sunrise and sunset occur at *angle* 0.
// Ref: http://www.nrel.gov/docs/fy08osti/34302.pdf (A.2.4.)
double _sunAngleTimeRel(float lat, float angle, double decl)
{
	return  1.0 / 15.0 * dm_arccos((-dm_sin(angle) - dm_sin(decl) * dm_sin(lat)) / 
		(dm_cos(decl) * dm_cos(lat)));
}

// Compute the time at which sun reaches a specific angle below horizon relative to sundial noon
double sunAngleTime(double sun_decl, double sun_eqt, float lat, float angle, short clock_dir) 
{
	double noon, t;
	
	noon = dm_fixHour(12.0 - sun_eqt); // mid-day time as per the Sun's exact noon position
	t = _sunAngleTimeRel(lat, angle, sun_decl);
	
	return noon + (clock_dir == DIR_COUNTER_CLOCKWISE ? -t : t);
}

// Compute asr time relative to sundial noon
double asrTime(double sun_decl, double sun_eqt, float lat, float shadow_factor)
{ 
	double angle, noon, t;
	
	// the Sun angle
	// factor = 1: According to the majority of schools (including Shafi'i, Maliki, Ja'fari, and Hanbali))
	//   The angle of the Sun when the length of an object's shadow is the same with
	//   the object's length plus the length of the object's shadow at noon.
	// factor = 2: Hanafi school
	//   The angle of the Sun when the length of an object's shadow is double
	//   the object's length plus the length of the object's shadow at noon.
	// [shadow_factor]
	//   1 to derive 45 degree for equal shadow and object length
	//   2 to derive ~64 degree for double shadow length
	// [lat - sun_decl]
	//   to derive the object's shadow length at sundial noon
	angle = -dm_arccot(shadow_factor + dm_tan(p_abs(lat - sun_decl)));	
	
	noon = dm_fixHour(12.0 - sun_eqt); // mid-day as per the Sun exact noon position
	t = _sunAngleTimeRel(lat, angle, sun_decl);
	
	return noon + t;	
}

// Sun angle adjustment for sunset/sunrise
// *elv* in meters.
double horizonAdj(float elv)
{
	double elv_angle = 0.0347 * p_sqrt(elv); // an approximation of elevation angle
	return 0.833 + elv_angle; // actual sunrise or sunset adjusted to the refraction of light
}

//---------------------- Higher latitudes -----------------------

// Adjust a time for higher latitudes 
double highLatTime(struct _ptimes *pt, double t, double base, float angle, double night, short clock_dir)
{
	double td, p = 0.5; // HIGHLAT_NIGHT_MIDDLE
	
	if (pt->high_lats == HIGHLAT_NONE)
		return t;
	
	if (pt->high_lats == HIGHLAT_ANGLE_BASED && angle != 0.0)
		p = 1.0 / 60.0 * angle;
	
	if (pt->high_lats == HIGHLAT_ONE_SEVEN)
		p = 1.0 / 7.0;
	
	p *= night; // night portion
	
	td = clock_dir == DIR_COUNTER_CLOCKWISE ? dm_fixHour(base - t) : dm_fixHour(t - base);
	
	if (t == p_nan() || td > p)
		t = base + (clock_dir == DIR_COUNTER_CLOCKWISE ? -p : p);
	
	return t;
}

//---------------------- Culculate prayer times -----------------------

// Convert Gregorian date to Julian day
// Ref: Astronomical Algorithms by Jean Meeus
double julian(short year, short month, short day)
{
	short A, B;
	long C;
	int D;
	
	if (month <= 2)
	{
		year -= 1;
		month += 12;
	}
	
	A = (short)(year / 100);
	B = 2 - A + (short)(A / 4);
	C = (long)(365.25 * (year + 4716));
	D = (int)(30.6001 * (month + 1));

	return C + D + day + B - 1524.5;
}

// Convert decimal time to h:m:s.
void t2hms(double t, short *h, short *m, short *s)
{
	while(t >= 24.0)
		t -= 24.0;
		
	*h = (short)t;
	t -= *h;
	t *= 60.0;
	*m = (short)t;
	t -= *m;
	t *= 60;
	*s = (short)t;
	t -= *s;
	
	// round up
	if (t >= 0.5)
	{
		if (++(*s) >= 60)
		{
			*s -= 60;
			
			if (++(*m) >= 60)
			{
				*m -= 60;
				if (++(*h) >= 24)
					*h -= 24;
			}
		}
	}	
}

void ptInit(struct _ptimes *pt)
{
	pt->imsak_rel_d = 0.0;
	pt->imsak_rel_m = 10.0;
	pt->fajr_rel_m = 0.0; 
	pt->dhuhr_rel_m = 0.0;
	pt->asr_rel_m = 0.0;
	pt->asr_factor = ASR_STANDARD;
	pt->high_lats = HIGHLAT_NIGHT_MIDDLE;

	// Muslim World League method
	pt->fajr_rel_d = 18.0;
	pt->maghrib_rel_d = 0.0;
	pt->maghrib_rel_m = 0.0;
	pt->isha_rel_d = 17.0;
	pt->isha_rel_m = 0.0;
	pt->midnight_type = MIDNIGHT_STANDARD;

	pt->elv = 0.0;
	pt->tz = 0.0;

	pt->phase = 0;
	pt->decl = 0.0;
	pt->eqt = 0.0;
	pt->horz_adj = 0.0;
	pt->night = 0.0;
}

void ptSetLocation(struct _ptimes *pt, float lat, float lng, float elv, float tz)
{
	pt->lat = lat;
	pt->lng = lng;
	pt->elv = elv;
	pt->tz = tz;
}

void ptSetDate(struct _ptimes *pt, short year, short month, short day)
{
	pt->year = year;
	pt->month = month;
	pt->day = day;
	pt->jd = julian(year, month, day); // - pt->lng / (15.0 * 24.0);
}

// Calculate prayer times.
// This is a self-threaded function designed for embedded system.
// The maths may be slow on tiny processors and hog other processes.
// Call this function multiple times until it return 0.
// The system can do any other things in between this scalls.
short ptCalc(struct _ptimes *pt)
{
	double td;

	switch (pt->phase)
	{
		case 0:
		
			pt->horz_adj = horizonAdj(pt->elv);
			break;
			
		case 1:
			
			sunPosition(pt->jd + DAYTIME_SUNRISE, &(pt->decl), &(pt->eqt));
			break;

		case 2:
		
			pt->sunrise = sunAngleTime(pt->decl, pt->eqt, pt->lat, pt->horz_adj, DIR_COUNTER_CLOCKWISE);
			break;

		case 3:
		
			sunPosition(pt->jd + DAYTIME_SUNSET, &(pt->decl), &(pt->eqt));
			break;

		case 4:
		
			pt->sunset = sunAngleTime(pt->decl, pt->eqt, pt->lat, pt->horz_adj, DIR_CLOCKWISE);
			break;
		
		case 5:
		
			pt->night = dm_fixHour(pt->sunrise - pt->sunset);
			break;
		
		case 6:
		
			sunPosition(pt->jd + DAYTIME_FAJR, &(pt->decl), &(pt->eqt));
			break;
		
		case 7:
		
			if (pt->fajr_rel_d != 0.0)
				sunPosition(pt->jd + DAYTIME_FAJR, &(pt->decl), &(pt->eqt));
		
			break;
		
		case 8:
		
			if (pt->fajr_rel_d != 0.0)
				pt->fajr = sunAngleTime(pt->decl, pt->eqt, pt->lat, pt->fajr_rel_d, DIR_COUNTER_CLOCKWISE);
		
			break;

		case 9:
		
			if (pt->fajr_rel_d == 0.0)
				pt->fajr = pt->sunrise - pt->fajr_rel_m / 60.0;

			break;
		
		case 10:
		
			if (pt->imsak_rel_d != 0.0)
				sunPosition(pt->jd + DAYTIME_IMSAK, &(pt->decl), &(pt->eqt));
		
			break;
		
		case 11:
		
			if (pt->imsak_rel_d != 0.0)
				pt->imsak = sunAngleTime(pt->decl, pt->eqt, pt->lat, pt->imsak_rel_d, DIR_COUNTER_CLOCKWISE);
		
			break;
		
		case 12:
		
			if (pt->imsak_rel_d == 0.0)
				pt->imsak = pt->fajr - pt->imsak_rel_m / 60.0;
		
			break;
		
		case 13:
		
			sunPosition(pt->jd + DAYTIME_DHUHR, &(pt->decl), &(pt->eqt));
			break;

		case 14:
		
			pt->dhuhr = dm_fixHour(12.0 - pt->eqt);
			pt->dhuhr += pt->dhuhr_rel_m / 60.0;
			break;
		
		case 15:
		
			sunPosition(pt->jd + DAYTIME_ASR, &(pt->decl), &(pt->eqt));
			break;

		case 16:
		
			pt->asr = asrTime(pt->decl, pt->eqt, pt->lat, pt->asr_factor);
			pt->asr += pt->asr_rel_m / 60.0;
			break;
		
		case 17:
			
			if (pt->maghrib_rel_d != 0.0)
				sunPosition(pt->jd + DAYTIME_MAGHRIB, &(pt->decl), &(pt->eqt));

			break;
		
		case 18:
			
			if (pt->maghrib_rel_d != 0.0)
				pt->maghrib = sunAngleTime(pt->decl, pt->eqt, pt->lat, pt->maghrib_rel_d, DIR_CLOCKWISE);

			break;
		
		case 19:
			
			if (pt->maghrib_rel_d == 0.0)
				pt->maghrib = pt->sunset + (pt->maghrib_rel_m / 60.0); 

			break;
			
		case 20:
			
			if (pt->isha_rel_d != 0.0)
				sunPosition(pt->jd + DAYTIME_ISHA, &(pt->decl), &(pt->eqt));

			break;
			
		case 21:
			
			if (pt->isha_rel_d != 0.0)
				pt->isha = sunAngleTime(pt->decl, pt->eqt, pt->lat, pt->isha_rel_d, DIR_CLOCKWISE);

			break;
			
		case 22:
			
			if (pt->isha_rel_d == 0.0)
				pt->isha = pt->maghrib + (pt->isha_rel_m / 60.0); 

			break;

		case 23:

			pt->imsak = highLatTime(pt, pt->imsak, pt->sunrise, 0.0, pt->night, DIR_COUNTER_CLOCKWISE);
			break;

		case 24:
		
			pt->fajr = highLatTime(pt, pt->fajr, pt->sunrise, pt->fajr_rel_d, pt->night, DIR_COUNTER_CLOCKWISE);
			break;
		
		case 25:
			
			pt->maghrib = highLatTime(pt, pt->maghrib, pt->sunset, pt->maghrib_rel_d, pt->night, DIR_CLOCKWISE);
			break;
			
		case 26:
			
			pt->isha = highLatTime(pt, pt->isha, pt->sunset, pt->isha_rel_d, pt->night, DIR_CLOCKWISE);
			break;
			
		case 27:
			
			if (pt->midnight_type == MIDNIGHT_JAFARI)
				pt->midnight = pt->sunset + dm_fixHour(pt->fajr - pt->sunset) / 2.0;
			else
				pt->midnight = pt->sunset + dm_fixHour(pt->sunrise - pt->sunset) / 2.0;
			
			break;
			
		case 28:
		
			// make local times
			td = pt->tz - pt->lng / 15.0;
			
			pt->imsak += td;
			pt->fajr += td;
			pt->sunrise += td;
			pt->dhuhr += td;
			pt->asr += td;
			pt->sunset += td;
			pt->maghrib += td;
			pt->isha += td;
			pt->midnight += td;
			
			break;
	}

	if (++(pt->phase) > 28)
		pt->phase = 0;
	
	return pt->phase;
}

