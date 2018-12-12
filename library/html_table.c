/* html_table.c 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver html_table ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "html_table.h"
#include "timlib.h"
#include "piece.h"

HTML_TABLE *new_html_table( char *title, char *sub_title )
{
	HTML_TABLE *h = (HTML_TABLE *)calloc( 1, sizeof( HTML_TABLE ) );
	h->title = title;
	h->sub_title = sub_title;
	h->heading_list = list_new();
	h->data_list = list_new();
	return h;
}

HTML_TABLE *html_table_new( char *title, char *sub_title, char *sub_sub_title )
{
	HTML_TABLE *h = (HTML_TABLE *)calloc( 1, sizeof( HTML_TABLE ) );
	h->title = title;
	h->sub_title = sub_title;
	h->sub_sub_title = sub_sub_title;
	h->heading_list = list_new();
	h->data_list = list_new();
	return h;
}

void html_table_output_table_heading(	char *title,
					char *sub_title )
{
	if ( title && *title )
		printf( "<h1>%s</h1>\n", title );

	if ( sub_title && *sub_title )
		printf( "<h2>%s</h2>\n", sub_title );

	printf( "<table border>\n" );
}

void html_table_output_table_heading2(	char *title,
					char *sub_title,
					char *sub_sub_title )
{
	if ( title && *title )
		printf( "<h1>%s</h1>\n", title );

	if ( sub_title && *sub_title )
		printf( "<h2>%s</h2>\n", sub_title );

	if ( sub_sub_title && *sub_sub_title )
		printf( "<h3>%s</h3>\n", sub_sub_title );

	printf( "<table border>\n" );
}

void html_table_output_data_heading( 	LIST *heading_list,
					int number_left_justified_columns,
					int number_right_justified_columns,
					LIST *justify_list )
{
	int column_number = 0;
	char buffer[ 1024 ];
	char *heading;

	if ( justify_list ) list_rewind( justify_list );

	printf( "<tr>\n" );
	if ( list_rewind( heading_list ) )
	{
		do {
			heading = list_get_string( heading_list );

			if ( character_exists( heading, '&' )
			&&   character_exists( heading, ';' ) )
			{
				strcpy( buffer, heading );
			}
			else
			{
				format_initial_capital(
					buffer,
					heading );
			}

			if ( justify_list )
			{
				printf( "<th align=%s>%s",
					list_get_string( justify_list ),
					buffer );
			}
			else
			{
				printf( "<th align=%s>%s\n", 
					html_table_get_alignment(
						column_number++,
						number_left_justified_columns,
						number_right_justified_columns),
					buffer );
			}

			printf( "</th>\n" );

			if ( justify_list ) list_next( justify_list );
		} while( list_next( heading_list ) );
	}
} /* html_table_output_data_heading() */

void html_table_output_data( 	LIST *data_list,
				int number_left_justified_columns,
				int number_right_justified_columns,
				int background_shaded,
				LIST *justify_list )
{
	char buffer[ 65536 ];
	int column_number = 0;

	if ( justify_list ) list_rewind( justify_list );
	printf( "<tr>\n" );

	if ( !list_rewind( data_list ) ) return;

	do {
		strcpy( buffer, list_get_string( data_list ) );
		printf( "<td" );

		if ( background_shaded )
			printf( " bgcolor=\"#CCCCCC\"" );

		if ( justify_list )
		{
			printf( " align=%s>\n",
				list_get_string( justify_list ) );
		}
		else
		{
			printf( " align=%s>",
				html_table_get_alignment(
				column_number++,
				number_left_justified_columns,
				number_right_justified_columns ) );
		}

		search_replace_string(
			buffer,
			"%0D%0A",
			"<br>" );

		printf( "%s</td>\n", buffer );

		if ( justify_list ) list_next( justify_list );

	} while( list_next( data_list ) );

} /* html_table_output_data() */

void html_table_set_heading( HTML_TABLE *d, char *heading )
{
	list_append_string( d->heading_list, heading );
}

void html_table_set_record_data(	HTML_TABLE *d,
					char *record_data,
					char delimiter )
{
	char data[ 65536 ];
	int i;

	if ( !delimiter )
	{
		html_table_set_data( d->data_list, strdup( record_data ) );
	}
	else
	for( i = 0; piece( data, delimiter, record_data, i ); i++ )
	{
		html_table_set_data( d->data_list, strdup( data ) );
	}

} /* html_table_set_record_data() */

void html_table_set_data( LIST *data_list, char *data )
{
	list_append_pointer( data_list, data );
}

void html_table_reset_data_heading( HTML_TABLE *d )
{
	d->heading_list = list_new();
}

void html_table_close( void )
{
	printf( "</table>\n" );
}

void html_table_set_number_left_justified_columns( HTML_TABLE *d, int i )
{
	d->number_left_justified_columns = i;
}

void html_table_set_number_right_justified_columns( HTML_TABLE *d, int i )
{
	d->number_right_justified_columns = i;
}

void html_table_set_heading_list( HTML_TABLE *html_table, LIST *heading_list )
{
	html_table->heading_list = heading_list;
}

void html_table_set_background_shaded(	HTML_TABLE *d )
{
	d->background_shaded = 1;
}

void html_table_set_background_unshaded(HTML_TABLE *d )
{
	d->background_shaded = 0;
}

char *html_table_get_alignment(	int column_number,
				int number_left_justified_columns,
				int number_right_justified_columns )
{
	if ( number_left_justified_columns
	&&   column_number < number_left_justified_columns )
	{
		return "left";
	}
	else
	if ( number_right_justified_columns
	&&   column_number < number_left_justified_columns +
			     number_right_justified_columns )
	{
		return "right";
	}
	else
	{
		return "left";
	}
} /* html_table_get_alignment() */

