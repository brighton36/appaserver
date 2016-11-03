/* invest2:$root/library/tim/dollar.c */
/* ---------------------------------- */

#include <stdio.h>
#include "piece.h"

/********************************************************************/
/*BOT
MONEY_DOUBLE_TO_TEXT()

Description:	This routine will take a double precision number and 
		convert it to its equivilant text.

Declaration:	char *money_double_to_text(amount);

Parameters:	'amount' (a double) is the number to convert.

Example:	double amount = 1234567
		printf("%s\n", money_double_to_text(amount));
		prints: 
		twelve thousand three hundred forty five and 67/100 dollars

Note:		Numbers that are negative or indeterminable will return a 
		value of zero.

EOT*/


void num_str ( num , buff )
int	num;					/* number to convert	*/
char	*buff;
{
	static char *ones[]=
	{
		"invalid ones ",
		"ONE ",
		"TWO ",
		"THREE ",
		"FOUR ",
		"FIVE ",
		"SIX ",
		"SEVEN ",
		"EIGHT ",
		"NINE "
	};
	static char *teen[]=
	{
		"TEN ",
		"ELEVEN ",
		"TWELVE ",
		"THIRTEEN ",
		"FOURTEEN ",
		"FIFTEEN ",
		"SIXTEEN ",
		"SEVENTEEN ",
		"EIGHTEEN ",
		"NINETEEN "
	};
	static char *tens[]=
	{
		"invalid tens0 ",
		"invalid tens1 ",
		"TWENTY ",
		"THIRTY ",
		"FORTY ",
		"FIFTY ",
		"SIXTY ",
		"SEVENTY ",
		"EIGHTY ",
		"NINETY "
	};

		if ( num >= 100 )
		{
			strcat ( buff , ones [ num/100 ] );
			strcat ( buff , "HUNDRED " );
			num = num % 100;
		}
		if ( num >= 20 )
		{
			strcat ( buff , tens [ num/10 ] );
			num = num % 10;
		}
		if ( num >= 10 )
		{
			strcat ( buff , teen [ num % 10 ] );
			num = 0;
		}
		if ( num >= 1 )
		{
			strcat ( buff , ones [ num ] );
			num = 0;
		}

} /* num_str() */

void do_cents(num,buff)
int num;
char *buff;
{
	char temp[100];

	if ( !num )
		strcpy( temp, "0" );
	else
		sprintf( temp, "%d", num );

	strcat(buff,"AND ");
	strcat(buff,temp);
	strcat(buff,"/100");
}


char *money_double_to_text( destination, num )
char *destination;
double num;
{
	void num_str(), do_cents();
	char temp_str[20];
	char piece_buffer[ 128 ];
	int temp;

	destination[0] = '\0';

	sprintf(temp_str,"%13.2f",num);

	if (!(int)num)			/* if no dollars then write ZERO */
		strcpy(destination,"ZERO ");

	if (num > 1000){
		temp = (int)(num / 1000);
		num_str(temp,destination);
		strcat(destination,"THOUSAND ");
		num = num - (int)(num/1000) * 1000;
	}

	num_str((int)num,destination);
	do_cents(atoi(piece( piece_buffer, '.',temp_str,1)),destination);

	return destination;
}

