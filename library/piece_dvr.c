/* piece_dvr.c */
/* ----------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

void test_escaped_delimiter( void );
void test_piece_shift_left( void );
void test_piece_shift_right( void );
void test_piece_replace( void );
void test_piece_delimiter_position( void );
void test_piece_quoted_string( void );
void test_piece_delete( void );
void test_piece_delete2( void );
void test_piece_unknown( void );
void test_piece_string( void );
void test_piece_delete_quote_comma( void );
void test_piece_insert_quote_comma( void );
void test_piece_replace_quote_comma( void );

int main()
{
	/* test1(); */
	/* test2(); */
	/* test_delimiter_tab(); */
	/* test_escaped_delimiter(); */
	/* test_swap(); */
	/* test_piece_insert(); */
	/* test_piece_multiple(); */
	/* test_piece_shift_left(); */
	/* test_piece_shift_right(); */
	/* test_piece_delimiter_position(); */
	/* test_piece_replace(); */
	/* test_piece_delete(); */
	/* test_piece_delete2(); */
	/* test_piece_unknown(); */
	test_piece_quoted_string();
	/* test_piece_string(); */
	/* test_piece_delete_quote_comma(); */
	/* test_piece_insert_quote_comma(); */
	/* test_piece_replace_quote_comma(); */

	return 0;
}

void test_piece_shift_left()
{
	char str[ 100 ];

	strcpy( str, "zero,one,two,three,four" );
	printf( "should be one,two,three,four,zero: is = %s\n",
		piece_shift_left( str, ',' ) );
}

void test_piece_shift_right()
{
	char str[ 100 ];

	strcpy( str, "zero,one,two,three,four" );
	printf( "should be four,zero,one,two,three: is = %s\n",
		piece_shift_right( str, ',' ) );
}

void test_escaped_delimiter()
{
	char str[ 100 ];
	char buffer[ 100 ];

	sprintf( str, "label: Note\\: this should be one piece" );
	printf( "str = (%s)\n", str );
	printf( "should be: (Note: this should be one piece)\n" );
	piece( buffer, ':', str, 1 );
	printf( "is:        (%s)\n", buffer );

	sprintf( str, "label= Note\\= this should be one piece" );
	printf( "str = (%s)\n", str );
	printf( "should be: (Note= this should be one piece)\n" );
	piece( buffer, '=', str, 1 );
	printf( "is:        (%s)\n", buffer );

}

#ifdef NOT_DEFINED
test1()
{
	char str[ 100 ];
	char buffer[ 100 ];

	strcpy( str, "zero|one|two|three" );
	printf( "str = (%s)\n", str );
	printf( "should be: (zero)\n" );
	printf( "is:        (%s)\n", 
		piece_split_in_two( buffer, '|', str, 0 ) );

	printf( "should be: (one|two|three)\n" );
	printf( "is:        (%s)\n", 
		piece_split_in_two( buffer, '|', str, 1 ) );

}

test2()
{
	char str[ 100 ];
	char buffer[ 100 ];

	strcpy( str, "zero|one|two|three" );
	printf( "str = (%s)\n", str );
	printf( "should be: (two)\n" );
	piece( buffer, '|', str, 2 );
	printf( "is:        (%s)\n", buffer );

}


test_delimiter_tab()
{
	char str[ 100 ];
	char buffer[ 100 ];

	sprintf( str, "zero\tone\ttwo\tthree" );
	printf( "str = (%s)\n", str );
	printf( "should be: (two)\n" );
	piece( buffer, 't', str, 2 );
	printf( "is:        (%s)\n", buffer );
}

test_swap()
{
	char buffer[ 100 ];

	strcpy( buffer, "zero|one|two" );
	printf( "piece_swap should be (zero|two|one) is (%s)\n",
		piece_swap( buffer, '|', 1, 2 ) );

	strcpy( buffer, "station,date,time,datatype,value" );
	printf( 
	"piece_swap should be (station,datatype,time,date,value) is (%s)\n",
		piece_swap( buffer, ',', 1, 3 ) );
	printf( 
	"piece_swap should be (station,datatype,date,time,value) is (%s)\n",
		piece_swap( buffer, ',', 2, 3 ) );
}

test_piece_insert()
{
	char buffer[ 100 ];

	strcpy( buffer, "zero|one|two" );
	printf( "piece_insert returned: (%s)\n",
		piece_insert( buffer, '|', "insert", 0 ) );

	strcpy( buffer, "zero|one|two" );
	printf( "piece_insert returned: (%s)\n",
		piece_insert( buffer, '|', "insert", 1 ) );

	strcpy( buffer, "zero|one|two" );
	printf( "piece_insert returned: (%s)\n",
		piece_insert( buffer, '|', "insert", 2 ) );

	strcpy( buffer, "zero|one|two" );
	printf( "piece_insert returned: (%s)\n",
		piece_insert( buffer, '|', "insert", 3 ) );
}

test_piece_multiple()
{
	char buffer[ 100 ];

	printf( "should be (zero|one) is = (%s)\n",
		piece_multiple( buffer, '|', "zero|one|two", 2 ) );
}

void test_piece_delimiter_position( void )
{
	char *string = "zero|one|two";

	printf( "for string = (%s), piece_offset = 0, should be 4: is = %d\n",
		string,
		piece_delimiter_position(
			string,
			'|',
			0 ) );

	printf( "for string = (%s), piece_offset = 1, should be 8: is = %d\n",
		string,
		piece_delimiter_position(
			string,
			'|',
			1 ) );

	printf( "for string = (%s), piece_offset = 2, should be -1: is = %d\n",
		string,
		piece_delimiter_position(
			string,
			'|',
			2 ) );

} /* test_piece_delimiter_position() */
#endif


void test_piece_delete( void )
{
	char s[ 128 ];

	strcpy( s, "zero|one|two" );
	printf(
"%s(): piece_delete( \"zero|one|two\", 0 ) should be (one|two) is (%s)\n",
		__FUNCTION__,
		piece_delete( s, '|', 0 ) );

	strcpy( s, "zero|one|two" );
	printf(
"%s(): piece_delete( \"zero|one|two\", 1 ) should be (zero|two) is (%s)\n",
		__FUNCTION__,
		piece_delete( s, '|', 1 ) );

	strcpy( s, "zero|one|two" );
	printf( "%s(): should be (zero|one) is (%s)\n",
		__FUNCTION__,
		piece_delete( s, '|', 2 ) );
}

void test_piece_replace( void )
{
	char str[ 100 ];

	strcpy( str, "zero,one,two,three,four" );
	printf( "should be zero,one,two,nine,four: is = %s\n",
		piece_replace( str, ',', "nine", 3 ) );
}

void test_piece_delete2( void )
{
	char s[ 256 ];

	strcpy( s, 
"12,6,Fall10,barnes_sound,bb,0,8,5,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,1," );

	printf(
		"%s(): piece_delete(20) is (%s)\n",
		__FUNCTION__,
		piece_delete( s, ',', 20 ) );

}

void test_piece_unknown( void )
{
	char *string;
	char destination[ 16 ];
	char delimiter;

	string = "\"zero\"";
	printf( "got string = (%s)\n", string );

	delimiter = 0;
	printf( "should be zero: is (%s)\n",
		piece_unknown(	destination,
				&delimiter,
				string,
				0 ) );

	string = "\"zero\",\"one\",\"two\"";
	printf( "got string = (%s)\n", string );

	delimiter = 0;
	printf( "should be one: is (%s)\n",
		piece_unknown(	destination,
				&delimiter,
				string,
				1 ) );

	string = "\"zero\",\"one\",\"two\"";
	printf( "got string = (%s)\n", string );

	delimiter = 0;
	printf( "should be two: is (%s)\n",
		piece_unknown(	destination,
				&delimiter,
				string,
				2 ) );
}

void test_piece_string( void )
{
	char *string;
	char destination[ 16 ];
	char *delimiter;

	string = "\"zero\",\"one\",\"two\"";
	printf( "got string = (%s)\n", string );
	delimiter = "\",\"";

	printf( "should be zero: is (%s)\n",
		piece_string(	destination,
				delimiter,
				string,
				0 ) );

	printf( "should be one: is (%s)\n",
		piece_string(	destination,
				delimiter,
				string,
				1 ) );

	printf( "should be two: is (%s)\n",
		piece_string(	destination,
				delimiter,
				string,
				2 ) );

	printf( "should be null: is (%s)\n",
		piece_string(	destination,
				delimiter,
				string,
				3 ) );

}

void test_piece_delete_quote_comma( void )
{
	char *string;
	char destination[ 16 ];

	string = "";
	printf( "got string = (%s)\n", string );

	printf( "should be (): is (%s)\n",
		piece_delete_quote_comma(
					destination,
					string,
					0 ) );

	string = "\"zero\"";
	printf( "got string = (%s)\n", string );

	printf( "should be (): is (%s)\n",
		piece_delete_quote_comma(
					destination,
					string,
					0 ) );

	string = "\"zero\",\"one\",\"two\"";
	printf( "got string = (%s)\n", string );

	printf( "should be (\"one\",\"two\"): is (%s)\n",
		piece_delete_quote_comma(
					destination,
					string,
					0 ) );

	printf( "should be (\"zero\",\"two\"): is (%s)\n",
		piece_delete_quote_comma(
					destination,
					string,
					1 ) );

	printf( "should be (\"zero\",\"one\"): is (%s)\n",
		piece_delete_quote_comma(
					destination,
					string,
					2 ) );

	printf( "should be null: is (%s)\n",
		piece_delete_quote_comma(
					destination,
					string,
					3 ) );

}

void test_piece_insert_quote_comma( void )
{
	char *source;
	char destination[ 128 ];

	source = "";
	printf( "got source = (%s)\n", source );

	printf( "should be (\"zero\"): is (%s)\n",
		piece_insert_quote_comma(
					destination,
					source,
					"zero",
					0 ) );

	source = "\"one\"";
	printf( "got source = (%s)\n", source );

	printf( "should be (\"zero\",\"one\"): is (%s)\n",
		piece_insert_quote_comma(
					destination,
					source,
					"zero",
					0 ) );

	source = "\"zero\",\"two\"";
	printf( "got source = (%s)\n", source );

	printf( "should be (\"zero\",\"one\",\"two\"): is (%s)\n",
		piece_insert_quote_comma(
					destination,
					source,
					"one",
					1 ) );

	source = "\"zero\",\"one\"";
	printf( "got source = (%s)\n", source );

	printf( "should be (\"zero\",\"one\",\"two\"): is (%s)\n",
		piece_insert_quote_comma(
					destination,
					source,
					"two",
					2 ) );

}

void test_piece_replace_quote_comma( void )
{
	char source_destination[ 128 ];
	char destination[ 128 ];
	char *source;

	strcpy( source_destination,
		"\"1AAA\",\"stage\",\"2013-06-20\",\"2400\",\"5\"" );

	piece_replace(	source_destination,
			PIECE_QUOTE_COMMA_DELIMITER_CODE,
			"2013-06-21",
			2 );
	printf( "returned = (%s)\n", source_destination );

	*source_destination = '\0';
	printf( "got source_destination = (%s)\n", source_destination );

	printf( "should be (\"zero\"): is (%s)\n",
		piece_replace(
			source_destination, 
			PIECE_QUOTE_COMMA_DELIMITER_CODE,
			"zero",
			0 ) );

	source = "\"one\"";
	printf( "got source = (%s)\n", source );

	printf( "should be (\"zero\"): is (%s)\n",
		piece_replace_quote_comma(
					destination,
					source,
					"zero",
					0 ) );

	source = "\"zero\",\"two\"";
	printf( "got source = (%s)\n", source );

	printf( "should be (\"zero\",\"one\"): is (%s)\n",
		piece_replace_quote_comma(
					destination,
					source,
					"one",
					1 ) );

	source = "\"zero\",\"one\",\"three\"";
	printf( "got source = (%s)\n", source );

	printf( "should be (\"zero\",\"one\",\"two\"): is (%s)\n",
		piece_replace_quote_comma(
					destination,
					source,
					"two",
					2 ) );
}

void test_piece_quoted_string( void )
{
	char buffer[ 128 ];
	char *string;
	char delimiter = 0;

#ifdef NOT_DEFINED
	string = "\"zero\"";
	printf( "got string = (%s)\n", string );
	printf( "should be zero: is (%s)\n",
		piece_quoted(	buffer,
				',',
				string,
				0,
				'"' ) );

	string =
"\"ENP\",\"P32462-21\",\"S174\",\"01/04/2007 10:00\",\"\",\"0.50\",\"\",\"2\",\"1\",\"2\",\"3246010\",\"25\",\"PHOSPHATE, TOTAL AS P\",\"following\",\".008\",\".002\",\"3\",\"0.008\",\"mg/L\",\"WMD\",\"WMD\",\"00665\",\"\",\"0.0080\",\"0.0020\",\"SM4500PF\",\"SAMP\",\"MON\",\"SW\",\"G\",\"0\",\"\",\"01/16/2007 12:48\",\"01/05/2007 09:46\",\"\",\"\"";

	printf( "got string = (%s)\n", string );

	printf(
"piece_offset = 4, should be (): is = (%s)\n",
		piece_quoted( buffer, ',', string, 4, '"' ) );

	printf(
"piece_offset = 12, should be (PHOSPHATE, TOTAL AS P): is = (%s)\n",
		piece_quoted( buffer, ',', string, 12, '"' ) );

	printf(
"piece_offset = 13, should be (following): is = (%s)\n",
		piece_quote_comma( buffer, string, 13 ) );
#endif

	string = "12-MONTH IRA CD (xxxxxx577-30),$4.43 ,\"$2,621.54 \",\"$2,621.54 \"";
	printf(
"piece_offset = 0, should be [12-MONTH IRA CD (xxxxxx577-30)] is = [%s]\n",
		piece_unknown( buffer, &delimiter, string, 0 ) );

	printf(
"piece_offset = 2, should be ($2,621.54): is = (%s)\n",
		piece_unknown( buffer, &delimiter, string, 2 ) );

} /* test_piece_quoted_string() */

