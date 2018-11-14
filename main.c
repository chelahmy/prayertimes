// main.c
// Prayer Times Calculator
// By Abdullah Daud, chelahmy@gmail.com
// 20 October 2018

#include "prayertimes.h"

int printf(const char *format, ...);

// Notes:
// 1. The main objective of this project is to convert the prayer time
//      algorithm from PrayTimes.js to C. See prayertimes.c.
// 2. prayertimes.c is meant for embedded systems. Hence, it is standalone
//      without the need of the standard math or any library.
// 

#define METHOD_MWL 0
#define METHOD_ISNA 1
#define METHOD_EGYPT 2
#define METHOD_MAKKAH 3
#define METHOD_KARACHI 4
#define METHOD_TEHRAN 5
#define METHOD_JAFARI 6

struct _pt_method_params
{
	float fajr_rel_d;
	float isha_rel_d;
	float isha_rel_m;
	float maghrib_rel_d;
	float maghrib_rel_m;
	short midnight_type;
};

struct _pt_method
{
	char *name;
	char *desc;
	struct _pt_method_params params;
};

struct _pt_method pt_methods[] =
{
	{
		.name = "MWL",
		.desc = "Muslim World League",
		.params =
		{
			.fajr_rel_d = 18.0,
			.isha_rel_d = 17.0,
			.isha_rel_m = 0.0,
			.maghrib_rel_d = 0,
			.maghrib_rel_m = 0,
			.midnight_type = MIDNIGHT_STANDARD
		}
	},
	{
		.name = "ISNA",
		.desc = "Islamic Society of North America (ISNA)",
		.params =
		{
			.fajr_rel_d = 15.0,
			.isha_rel_d = 15.0,
			.isha_rel_m = 0.0,
			.maghrib_rel_d = 0,
			.maghrib_rel_m = 0,
			.midnight_type = MIDNIGHT_STANDARD
		}
	},
	{
		.name = "Egypt",
		.desc = "Egyptian General Authority of Survey",
		.params =
		{
			.fajr_rel_d = 19.5,
			.isha_rel_d = 17.5, 
			.isha_rel_m = 0.0,
			.maghrib_rel_d = 0,
			.maghrib_rel_m = 0,
			.midnight_type = MIDNIGHT_STANDARD
		}
	},
	{
		.name = "Makkah",
		.desc = "Umm Al-Qura University, Makkah",
		.params =
		{
			.fajr_rel_d = 18.5, // fajr was 19 degrees before 1430 hijri
			.isha_rel_d = 0.0,
			.isha_rel_m = 90.0, 
			.maghrib_rel_d = 0.0,
			.maghrib_rel_m = 0,
			.midnight_type = MIDNIGHT_STANDARD
		}
	},
	{
		.name = "Karachi",
		.desc = "University of Islamic Sciences, Karachi",
		.params =
		{
			.fajr_rel_d = 18.0,
			.isha_rel_d = 18.0,
			.isha_rel_m = 0.0,
			.maghrib_rel_d = 0.0,
			.maghrib_rel_m = 0,
			.midnight_type = MIDNIGHT_STANDARD
		}
	},
	{
		.name = "Tehran",
		.desc = "Institute of Geophysics, University of Tehran",
		.params =
		{
			.fajr_rel_d = 17.7,
			.isha_rel_d = 14.0, // isha was not explicitly specified in this method
			.isha_rel_m = 0.0,
			.maghrib_rel_d = 4.5,
			.maghrib_rel_m = 0,
			.midnight_type = MIDNIGHT_JAFARI
		}
	},
	{
		.name = "Jafari",
		.desc = "Shia Ithna-Ashari, Leva Institute, Qum",
		.params =
		{
			.fajr_rel_d = 16.0,
			.isha_rel_d = 14.0,
			.isha_rel_m = 0.0,
			.maghrib_rel_d = 4.0,
			.maghrib_rel_m = 0,
			.midnight_type = MIDNIGHT_JAFARI
		}
	}
};

void ptSetMethod(struct _ptimes *pt, short method)
{
	if (method < 0 || method >= 7)
		return;
	
	struct _pt_method *m = &(pt_methods[method]);
	
	pt->fajr_rel_d = m->params.fajr_rel_d;
	pt->maghrib_rel_d = m->params.maghrib_rel_d;
	pt->maghrib_rel_m = m->params.maghrib_rel_m;
	pt->isha_rel_d = m->params.isha_rel_d;
	pt->isha_rel_m = m->params.isha_rel_m;
	pt->midnight_type = m->params.midnight_type;
}

// Compare the equality of *s1* and *s2* string.
int eq(char *s1, char *s2)
{
	while (*s1 != '\0' && *s2 != '\0')
	{
		if (*s1 != *s2)
			return 0;
			
		++s1;
		++s2;
	}
	
	if (*s1 == '\0' && *s2 == '\0')
		return 1;
		
	return 0;
}

// Trim string on the left by skipping whitespaces.
char *ltrim(char *s)
{
	while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')
		++s;
		
	return s;
}

// Check if *s* is a number.
short isnum(char *s)
{
	s = ltrim(s);
	
	if (*s == '-' || *s == '+')
		++s;

	if (*s >= '0' && *s < '9')
		return 1;

	return 0;
}

// Convert string to number.
double atof(char *s)
{
	double v = 0.0, dp = 0.0, m = 1.0;
	
	s = ltrim(s);
	
	if (*s == '-' || *s == '+')
	{
		if (*s == '-')
			m = -1.0;
		
		++s;
	}
		
	while (*s != '\0')
	{
		if (*s >= '0' && *s <= '9')
		{
			dp *= 10.0;
			v *= 10.0;
			v += (float)(*s - '0');
		}
		else if (*s == '.')
		{
			if (dp > 0.0)
				break;
				
			dp = 1.0;
		}
		else
			break;
		
		++s;
	}
	
	if (dp > 0.0)
		v /= dp;
		
	return v * m;
}

// Convert clock or compass notation to number.
// I.e. hh::mm::ss -> nn.nnnn
double ctof(char *s)
{
	short i = 0, m = 1;
	short p[3] = {0, 0, 0};
	
	s = ltrim(s);
		
	if (*s == '-' || *s == '+')
	{
		if (*s == '-')
			m = -1;
		
		++s;
	}
		
	while (*s != '\0')
	{
		if (*s >= '0' && *s <= '9')
		{
			p[i] *= 10;
			p[i] += (short)(*s - '0');
		}
		else if (*s == ':')
		{
			if (++i >= 3)
				break;
		}
		else
			break;
		
		++s;
	}
	
	while (p[1] > 60)
		p[1] -= 60;
	
	while (p[2] > 60)
		p[2] -= 60;
		
	return ((double)p[0] + ((double)p[1] / 60.0) + ((double)p[2] / 3600.0)) * (double)m;
}

// Split date notation <yyyy-mm-dd> to invidual
// *y*ear, *m*onth and *d*ay parts.
void ds(char *s, short *y, short *m, short *d)
{
	short i = 0, *p[3];
	
	p[0] = y;
	p[1] = m;
	p[2] = d;
	
	*p[0] = 0;
	*p[1] = 0;
	*p[2] = 0;

	s = ltrim(s);

	while (*s != '\0')
	{
		if (*s >= '0' && *s < '9')
		{
			*(p[i]) *= 10;
			*(p[i]) += (short)(*s - '0');
		}
		else if (*s == '-')
		{
			if (++i >= 3)
				break;
		}
		else
			break;

		++s;
	}		
}

// Parse arguments into *pt* structure.
void parseArgs(struct _ptimes *pt, int argc, char *argv[], short *out, short *pm)
{
	short i, a, b, c;
	char *s;
	float v;

	*pm = 0;
	
	for (i = 0; i < argc; i++)
	{
		if (eq(argv[i], "o")) // output
		{
			if (i + 1 < argc)
				*out = (short)ctof(argv[++i]);
		}
		else if (eq(argv[i], "l")) // location
		{
			if (i + 1 < argc)
			{
				pt->lat = (float)ctof(argv[++i]);
				
				if (i + 1 < argc)
				{
					pt->lng = (float)ctof(argv[++i]);
					
					if (i + 1 < argc && isnum(argv[i+1]))
						pt->elv = (float)atof(argv[++i]);
				}
			}
		}
		else if (eq(argv[i], "z")) // timezone
		{
			if (i + 1 < argc)
				pt->tz = (float)atof(argv[++i]);		
		}
		else if (eq(argv[i], "d")) // date
		{
			if (i + 1 < argc)
			{
				ds(argv[++i], &a, &b, &c);
				ptSetDate(pt, a, b, c);
			}
		}
		else if (eq(argv[i], "m")) // method
		{
			if (i + 1 < argc)
				ptSetMethod(pt, (short)atof(argv[++i]) - 1);
		}
		else if (eq(argv[i], "p")) // print method
		{
			if (i + 1 < argc)
				*pm = (short)atof(argv[++i]);
		}
		else if (eq(argv[i], "a")) // adjustment
		{
			if (i + 1 < argc)
			{
				a = (short)atof(argv[++i]); // prayer time

				if (i + 1 < argc)
				{
					s = argv[++i]; // value type

					if (i + 1 < argc)
					{
						v = (float)atof(argv[++i]); // value
						
						if (eq(s, "m"))
							switch (a)
							{
								case 1: pt->imsak_rel_m = v; break;
								case 2: pt->fajr_rel_m = v; break;
								case 3: pt->dhuhr_rel_m = v; break;
								case 4: pt->asr_rel_m = v; break;
								case 5: pt->maghrib_rel_m = v; break;
								case 6: pt->isha_rel_m = v; break;
							}
						else if (eq(s, "d"))
							switch (a)
							{
								case 1: pt->imsak_rel_d = v; break;
								case 2: pt->fajr_rel_d = v; break;
								case 5: pt->maghrib_rel_d = v; break;
								case 6: pt->isha_rel_d = v; break;
							}
					}
				}
			}
		}
		else if (eq(argv[i], "f")) // asr factor
		{
			if (i + 1 < argc)
				pt->asr_factor = (float)atof(argv[++i]);
		}
		else if (eq(argv[i], "n")) // midnight type
		{
			if (i + 1 < argc)
				pt->midnight_type = (short)atof(argv[++i]);
		}
		else if (eq(argv[i], "h")) // high latitude adjustment
		{
			if (i + 1 < argc)
				pt->high_lats = (short)atof(argv[++i]);
		}
	} 
}

void help(void)
{
	printf("PRAYER TIMES Version 0.1\n\n");
	printf("\tA tool to calculate muslim prayer times based on sundial\n");
	printf("\tapproximation of sunrise and noon. As for sunset, the time\n");
	printf("\tdifferent from sunrise to noon is almost the same as from\n");
	printf("\tnoon to sunset. All prayer times can be referred to those\n");
	printf("\tdaily events by applying standard constant values such as\n");
	printf("\tdegrees or minutes before sunrise or after sunset. However,\n");
	printf("\tsome schools of thoughts or locality apply minor adjustments\n");
	printf("\tto the constants according to their methods. Hence, there\n");
	printf("\tare a number of prayer time methods in the world. And the\n");
	printf("\talgorithm to calculate sunrise and noon is still evolving\n");
	printf("\tfor accuracy, speed and simplicity. Some tools use altered\n");
	printf("\talgorithms for reasons of computation limitations. Hence,\n");
	printf("\tthere are variations in minutes and seconds to the calculated\n");
	printf("\tprayer times by different tools. Yet, the variations can still\n");
	printf("\tbe minimized by readjusting the constants. This tool allows\n");
	printf("\tconstants to be adjusted.\n\n");
	printf("USAGE:\n");
	printf("\tpt <options>\n\n");
	printf("\toptions:\n");
	printf("\to <output type>\n");
	printf("\t   1 CSV\n");
	printf("\t   2 JSON\n");
	printf("\t   If no output type is specified then a normal form\n");
	printf("\t   of prayer times will be printed out.\n");
	printf("\tl <latitute> <longitude> [<elevation>]\n");
	printf("\t   lat/long: [+/-]deg:min:sec\n");
	printf("\tz <timezone>\n");
	printf("\td <yyyy-mm-dd>\n");
	printf("\tm <method>\n");
	printf("\t   1 Muslim World League\n");
	printf("\t     (Europe, Far East, parts of US)\n");
	printf("\t   2 Islamic Society of North America (ISNA)\n");
	printf("\t     (North America (US and Canada))\n");
	printf("\t   3 Egyptian General Authority of Survey\n");
	printf("\t     (Africa, Syria, Lebanon, Malaysia)\n");
	printf("\t   4 Umm al-Qura University, Makkah\n");
	printf("\t     (Arabian Peninsula)\n");
	printf("\t   5 University of Islamic Sciences, Karachi\n");
	printf("\t     (Pakistan, Afganistan, Bangladesh, India)\n");
	printf("\t   6 Institute of Geophysics, University of Tehran\n");
	printf("\t     (Iran, Some Shia communities)\n");
	printf("\t   7 Shia Ithna-Ashari, Leva Research Institute, Qum\n");
	printf("\t     (Some Shia communities worldwide)\n");
	printf("\t   http://praytimes.org/wiki/Calculation_Methods\n");
	printf("\t   Note: The methods above make specific adjustments\n");
	printf("\t   to prayer times which can be overridden by the\n");
	printf("\t   options below.\n");
	printf("\tp <method>\n");
	printf("\t   1-7 Print method details. See methods above.\n");
	printf("\t   8   Print all settings.\n");
	printf("\ta <prayer time> <m|d> <value>\n");
	printf("\t   adjust prayer time with *m*inutes or *d*egrees\n");
	printf("\t     minute value: [+/-]min:sec\n");
	printf("\t     degree value: [+/-]deg:min:sec\n");
	printf("\t   prayer time:\n");
	printf("\t   1 imsak   (degrees before sunrise, minutes before fajr)\n");
	printf("\t   2 fajr    (before sunrise)\n");
	printf("\t   3 dhuhr   (minutes after noon)\n");
	printf("\t   4 asr     (minutes adjustment)\n");
	printf("\t   5 maghrib (after sunset)\n");
	printf("\t   6 isha    (degrees after sunset, minutes after maghrib)\n");
	printf("\tf <asr shadow factor>\n");
	printf("\t   1 Shafi`i, Maliki, Ja`fari, Hanbali\n");
	printf("\t   2 Hanafi\n");
	printf("\tn <midnight>\n");
	printf("\t   1 Standard (middle of sunset and sunrise)\n");
	printf("\t   2 Jafari   (middle of sunset and fajr)\n");
	printf("\th <high latitude adjustment>\n");
	printf("\t   0 none\n");
	printf("\t   1 middle of night\n");
	printf("\t   2 angle/60th of night\n");
	printf("\t   3 1/7th of night\n");
	printf("\t   \n");
	printf("\tExample: pt l 1:43 103:32 z 8 d 2018-10-20 m 3\n");
	printf("\n");
	printf("By Abdullah Daud, chelahmy@gmail.com, 2018.\n");
	printf("Credits to PrayTimes.org\n\n");
}

void hms(double t, char end)
{
	short h, m, s;
	t2hms(t, &h, &m, &s);
	printf("%02d:%02d:%02d%c", h, m, s, end);	
}

int main(int argc, char *argv[])
{
	struct _ptimes pt;
	struct _pt_method *ptm;
	short out = 0, pm = 0;
		
	ptInit(&pt);
	ptSetLocation(&pt, 43, -80, 0, -5); // Waterloo, ON, Canada
	ptSetDate(&pt, 2018, 10, 17); // 17 October 2018

	if (argc < 2)
	{
		help();		
		return 0;
	}

	parseArgs(&pt, argc - 1, &(argv[1]), &out, &pm);

	while (ptCalc(&pt) != 0)
	{
		// do whatever
		// this is meant for embedded system self-threading
		// because ptCalc() is floating point intensive
	}

	if (pm > 0)
	{
		if (--pm >= 0 && pm < 7) // print method settings
		{ 
			ptm = &pt_methods[pm];
			printf("%s\n", ptm->desc);

			if (ptm->params.fajr_rel_d != 0.0)
				printf("Fajr %f degrees before sunrise.\n", ptm->params.fajr_rel_d);

			if (ptm->params.isha_rel_d != 0.0)
				printf("Isha %f degrees after sunset.\n", ptm->params.isha_rel_d);
			else if (ptm->params.isha_rel_m != 0.0)
				printf("Isha %f minutes after maghrib.\n", ptm->params.isha_rel_m);

			if (ptm->params.maghrib_rel_d != 0.0)
				printf("Maghrib %f degrees after sunset.\n", ptm->params.maghrib_rel_d);
			else if (ptm->params.maghrib_rel_m != 0.0)
				printf("Maghrib %f minutes after sunset.\n", ptm->params.maghrib_rel_m);
				
			if (ptm->params.midnight_type == MIDNIGHT_STANDARD)
				printf("Midnight standard - middle of sunset and sunrise.\n");
				
			if (ptm->params.midnight_type == MIDNIGHT_JAFARI)
				printf("Midnight Jafari - middle of sunset and fajr.\n");
		}
		else if (pm == 7) // print all settings
		{
			if (pt.imsak_rel_d != 0.0)
				printf("Imsak %0.2f degrees before sunrise.\n", pt.imsak_rel_d);
			else if (pt.imsak_rel_m != 0.0)
				printf("Imsak %0.2f minutes before fajr.\n", pt.imsak_rel_m);
			else
				printf("Imsak is not defined.");

			if (pt.fajr_rel_d != 0.0)
				printf("Fajr %0.2f degrees before sunrise.\n", pt.fajr_rel_d);
			else if (pt.fajr_rel_m != 0.0)
				printf("Fajr %0.2f minutes before sunrise.\n", pt.fajr_rel_m);
			else
				printf("Fajr is not defined.");

			if (pt.dhuhr_rel_m != 0.0)
				printf("Dhuhr %0.2f minutes after noon.\n", pt.dhuhr_rel_m);
			else
				printf("Dhuhur at noon.\n");

			if (pt.asr_factor == ASR_STANDARD)
				printf("Asr standard - shadow equals object height plus its shadow at noon.\n");
			else if (pt.asr_factor == ASR_HANAFI)
				printf("Asr Hanafi - shadow equals double object height plus its shadow at noon.\n");
			else if (pt.asr_factor != 0.0)
			{
				printf("Asr shadow factor is %0.2f (%0.2lf degree) plus object shadow at noon.\n", 
					pt.asr_factor, -dm_arccot(pt.asr_factor));
			}
			else
				printf("Asr shadow factor is not defined.\n");
				
			if (pt.asr_rel_m != 0.0)
				printf("Asr %0.2f minutes adjustment.\n", pt.asr_rel_m);

			if (pt.maghrib_rel_d != 0.0)
				printf("Maghrib %0.2f degrees after sunset.\n", pt.maghrib_rel_d);
			else if (pt.maghrib_rel_m != 0.0)
				printf("Maghrib %0.2f minutes after sunset.\n", pt.maghrib_rel_m);
			else
				printf("Maghrib at sunset.\n");

			if (pt.isha_rel_d != 0.0)
				printf("Isha %0.2f degrees after sunset.\n", pt.isha_rel_d);
			else if (pt.isha_rel_m != 0.0)
				printf("Isha %0.2f minutes after maghrib.\n", pt.isha_rel_m);
			else
				printf("Isha is not defined.");
				
			if (pt.midnight_type == MIDNIGHT_STANDARD)
				printf("Midnight standard - middle of sunset and sunrise.\n");
			else if (pt.midnight_type == MIDNIGHT_JAFARI)
				printf("Midnight Jafari - middle of sunset and fajr.\n");
				
			else if (pt.high_lats == HIGHLAT_NIGHT_MIDDLE)
				printf("High latitude adjustment to middle of night.\n");
			else if (pt.high_lats == HIGHLAT_ANGLE_BASED)
				printf("High latitude adjustment to 60th of angle.\n");
			if (pt.high_lats == HIGHLAT_ONE_SEVEN)
				printf("High latitude adjustment to 1/7th of night.\n");
			else
				printf("No high latitude adjustment.\n");
		}
		else
			printf("Invalid number\n");
	}
	else if (out == 1) // CSV
	{
		printf("%04d-%02d-%02d,", pt.year, pt.month, pt.day);
		hms(pt.imsak, ',');
		hms(pt.fajr, ',');
		hms(pt.sunrise, ',');
		hms(pt.dhuhr, ',');
		hms(pt.asr, ',');
		hms(pt.sunset, ',');
		hms(pt.maghrib, ',');
		hms(pt.isha, ',');
		hms(pt.midnight, ' ');
		printf("\n");
	}
	else if (out == 2) // JSON
	{
		printf("{\"date\":\"%04d-%02d-%02d\"", pt.year, pt.month, pt.day);
		printf(",\"imsak\":\"");		hms(pt.imsak, '"');
		printf(",\"fajr\":\"");		hms(pt.fajr, '"');
		printf(",\"sunrise\":\"");	hms(pt.sunrise, '"');
		printf(",\"dhuhr\":\"");		hms(pt.dhuhr, '"');
		printf(",\"asr\":\"");		hms(pt.asr, '"');
		printf(",\"sunset\":\"");	hms(pt.sunset, '"');
		printf(",\"maghrib\":\"");	hms(pt.maghrib, '"');
		printf(",\"isha\":\"");		hms(pt.isha, '"');
		printf(",\"midnight\":\"");	hms(pt.midnight, '"');
		printf("}\n"); 
	}
	else
	{	
		printf("----%04d-%02d-%02d----\n", pt.year, pt.month, pt.day);
		printf("lat %14.6f\nlng %14.6f\nelv %14.6f\n", pt.lat, pt.lng, pt.elv);
		printf("tz  %14.6f\n", pt.tz);
		printf("------------------\n");
		printf("imsak:    "); hms(pt.imsak, '\n');
		printf("fajr:     "); hms(pt.fajr, '\n');
		printf("sunrise:  "); hms(pt.sunrise, '\n');
		printf("dhuhr:    "); hms(pt.dhuhr, '\n');
		printf("asr:      "); hms(pt.asr, '\n');
		printf("sunset:   "); hms(pt.sunset, '\n');
		printf("maghrib:  "); hms(pt.maghrib, '\n');
		printf("isha:     "); hms(pt.isha, '\n');
		printf("midnight: "); hms(pt.midnight, '\n');
	}
	
	return 0;
}
