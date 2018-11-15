// prayertimes.h

#ifndef __PRAYERTIMES_H__
#define __PRAYERTIMES_H__

#define ASR_STANDARD 1.0		// Shafi`i, Maliki, Ja`fari, Hanbali
#define ASR_HANAFI 2.0			// Hanafi

#define MIDNIGHT_STANDARD 0		// Mid sunset to sunrise
#define MIDNIGHT_JAFARI 1		// Mid sunset to Fajr

#define HIGHLAT_NONE 0			// No adjustment
#define HIGHLAT_NIGHT_MIDDLE 1	// Middle of night
#define HIGHLAT_ANGLE_BASED 2	// Angle/60th of night
#define HIGHLAT_ONE_SEVEN 3		// 1/7th of night

#define DIR_CLOCKWISE 0
#define DIR_COUNTER_CLOCKWISE 1

// Time of the day adjustment to improve accuracy in calculating
// the Sun's declination angle and equation of time for a given date.
// See sunPosition().
#define DAYTIME_IMSAK 	0.208333 // 05:00h
#define DAYTIME_FAJR 	0.208333 // 05:00h
#define DAYTIME_SUNRISE 0.250000 // 06:00h
#define DAYTIME_DHUHR 	0.500000 // 12:00h
#define DAYTIME_ASR 	0.541667 // 13:00h
#define DAYTIME_SUNSET 	0.750000 // 18:00h
#define DAYTIME_MAGHRIB 0.750000 // 18:00h
#define DAYTIME_ISHA 	0.750000 // 18:00h

// pi
#define P_hPI  1.570796326794896
#define P_PI   3.141592653589793
#define P_3hPI 4.71238898038469
#define P_2PI  6.283185307179586

struct _ptimes
{
	double imsak;
	double fajr;
	double sunrise;
	double dhuhr;
	double asr;
	double sunset;
	double maghrib;
	double isha;
	double midnight;
	
	// settings
	//  x_rel_d : relative in degrees
	//  x_rel_m : relative in minutes
	float imsak_rel_d;
	float imsak_rel_m;
	float fajr_rel_d;
	float fajr_rel_m;
	float dhuhr_rel_m;
	float asr_factor; // shadow length against object height: 1 - equal, 2 - double (Hanafi). 
	float asr_rel_m;
	float maghrib_rel_d;
	float maghrib_rel_m;
	float isha_rel_d;
	float isha_rel_m;
	short midnight_type;
	short high_lats;

	// location and time
	float lat; // Latitude
	float lng; // Longitude
	float elv; // Elevation in meters
	float tz;  // Time zone
	short year;
	short month;
	short day;
	double jd; // Julian date
	
	// run time variables
	short phase;
	double decl;
	double eqt;
	double horz_adj;
	double night;
};

// Math
float p_nan(void);
double p_abs(double v);
double p_sqrt(double v);
double p_sin(double x);
double p_cos(double x);
double p_tan(double x);
double p_asin(double x);
double p_acos(double x);
double p_atan(double x);
double p_atan2(double y, double x);

// Degree-based math
double dm_dtr(double d);
double dm_rtd(double r);
double dm_sin(double d);
double dm_cos(double d);
double dm_tan(double d);
double dm_arcsin(double d);
double dm_arccos(double d);
double dm_arctan(double d);
double dm_arccot(double x);
double dm_arctan2(double y, double x);
double dm_fix(double a, double b);
double dm_fixAngle(double a);
double dm_fixHour(double a);

void sunPosition(double jd, double *decl, double *eqt);
double _sunAngleTimeRel(float lat, float angle, double decl);
double sunAngleTime(double sun_decl, double sun_eqt, float lat, float angle, short clock_dir);
double asrTime(double sun_decl, double sun_eqt, float lat, float shadow_factor);
double horizonAdj(float elv);
double highLatTime(struct _ptimes *pt, double t, double base, float angle, double night, short clock_dir);
double julian(short year, short month, short day);

// Convert decimal time to h:m:s.
void t2hms(double t, short *h, short *m, short *s);

void ptInit(struct _ptimes *pt);
void ptSetLocation(struct _ptimes *pt, float lat, float lng, float elv, float tz);
void ptSetDate(struct _ptimes *pt, short year, short month, short day);
short ptCalc(struct _ptimes *pt);

#endif

