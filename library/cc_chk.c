/* cc_chk.c */
/* -------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "cc_chk.h"

/* array of cc names */
/* ----------------- */
static char *cc_name_array[] = {
        "Visa",
        "MasterCard",
        "American Express",
        "Discover",
        "Diner's Club",
        "Carte Blanche",
        "Carte Blanche Intern'l" };

/* array of to identify credit card */
/* -------------------------------- */
static CC_ID_TYPE cc_id_array[] = {
        { 0, 13, '4', '\0', '\0' }, /* Visa */
        { 0, 16, '4', '\0', '\0' }, /* Visa */
        { 1, 16, '5', '\0', '\0' }, /* Master Card*/
        { 2, 15, '3',  '4', '\0' }, /* American Express */
        { 2, 15, '3',  '7', '\0' }, /* American Express */
        { 3, 16, '6',  '0',  '1' }, /* Discover Card */
        { 4, 14, '3',  '0', '\0' }, /* Diner's Club */
        { 4, 14, '3',  '6', '\0' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '1' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '2' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '3' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '4' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '5' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '6' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '7' }, /* Diner's Club */
        { 4, 14, '3',  '8',  '8' }, /* Diner's Club */
        { 5, 14, '3',  '8',  '9' }, /* Carte Blanche */
        { 6, 14, '9', '\0', '\0' }, /* Carte Blanche inter*/
        { 99, '\0' } };

/* strip_non_digits */
/* ---------------- */
void strip_non_digits( char *Dest, char *Src  )
{
        while( *Src )
        {
                if( isdigit( *Src ) )
                        *Dest++ = *Src++;
                else
                        Src++;
        } /* end while */
        *Dest = '\0';

} /* end strip_non_digit */

/* cc_identify()

        Identify credit card
        -------------------------------
        returns:-1 error
                0  Visa
                1  Master Card
                2  American Express
                3  Discover
                4  Diner's Club
                5  Carte Blanche
                6  Carte Blanche International
*/
int cc_identify( char *cc_string )
{
        char cc_number[ 80 ] = { 0 };
        int  cc_number_size;
        int  index;

        strip_non_digits( cc_number, cc_string );
        cc_number_size = strlen( cc_number );

        if( cc_number_size < MIN_CC_SIZE || cc_number_size > MAX_CC_SIZE )
                return -1;

        for(index = 0; cc_id_array[ index ].cc_id != 99; index++ )
        {
                if( cc_id_array[ index ].cc_size != ( char )cc_number_size )
                {
                        continue;
                }
                if( cc_id_array[ index ].first_digit != cc_number[ 0 ] )
                {
                        continue;
                }
                if( ( cc_id_array[ index ].secnd_digit != '\0' )
                && ( cc_id_array[ index ].secnd_digit != cc_number[ 1 ] ) )
                {
                        continue;
                }
                if( ( cc_id_array[ index ].third_digit != '\0' )
                && ( cc_id_array[ index ].third_digit != cc_number[ 2 ] ) )
                {
                        continue;
                }
                return cc_id_array[ index ].cc_id;
        } /* end for */

        return -1;

} /* end cc_identify */


/* cc_chk_digit()

        check the the digits of a credit card.
        --------------------------------------
        returns: 1 == valid
                 0 == invalid
*/
int cc_chk_digit( char *cc_string )
{
        char cc_number[ 80 ] = { 0 };
        int cc_number_size;
        int index;
        int weight = 2;
        int product;
        int current_number;
        int accummulator = 0;

        strip_non_digits( cc_number, cc_string );
        cc_number_size = strlen( cc_number );
        index = cc_number_size - 2;

        if( cc_number_size < MIN_CC_SIZE || cc_number_size > MAX_CC_SIZE )
                return 0;

        for( ; index >= 0; index-- )
        {
                if( !isdigit( cc_number[ index ]) )
                        continue;
                current_number = cc_number[ index ] - 48;
                product = current_number * weight;
                accummulator += product / 10;
                accummulator += product % 10;
                if( weight == 2 )
                        weight = 1;
                else
                        weight = 2;
        }
        accummulator %= 10;
        accummulator = 10 - accummulator;

        if( accummulator == 10 )
                accummulator = 0;

        if( accummulator == ( cc_number[ cc_number_size - 1 ] - 48 ) )
                return 1;
        return 0;

} /* end cc_chk_digit */


char *get_cc_name( int cc_number )
{
        if( ( cc_number >= 0 ) || ( cc_number <= 6 ) )
                return cc_name_array[ cc_number ];
        else
                return (char *)0;

} /* end get_cc_name */

