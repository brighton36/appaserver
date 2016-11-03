/* creel_dvr.c */
/* ----------- */

#include <stdio.h>
#include <stdlib.h>
#include "creel.h"

/*
#define BEGIN_DATE	"1958-06-01"
#define END_DATE	"2014-08-31"
*/
#define BEGIN_DATE	"1992-01-18"
#define END_DATE	"1992-01-18"

void test1();

int main()
{
	test1();
	return 0;
}

void test1()
{
	CREEL *creel;

	creel =
		creel_new(
			"creel",
			BEGIN_DATE,
			END_DATE,
			(char *)0 /* fishing_purpose */ );

}
