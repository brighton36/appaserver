#include <stdio.h>
#include <stdlib.h>
#include "update_statement.h"
#include "list.h"

int main()
{
	LIST *attribute_list;
	LIST *data_list;
	LIST *primary_attribute_list;
	LIST *primary_data_list;
	char update_statement_string[ 4096 ];
	UPDATE_STATEMENT *u;

	attribute_list = 
		string2list( "measurement_date|measurement_time", '|' );

	data_list = 
		string2list( "2000-01-01|0020", '|' );

	primary_attribute_list = 
	string2list( "station|datatype|measurement_date|measurement_time",'|');

	primary_data_list = 
	string2list( "BA|stage|1999-12-31|2320", '|' );

	u = new_update_statement( "measurement" );
	update_statement_set_attribute_list( u, attribute_list );
	update_statement_set_data_list( u, data_list );

	update_statement_set_primary_attribute_list(
					u, primary_attribute_list );

	update_statement_set_primary_data_list(
					u, primary_data_list );

	if ( !update_statement_get( update_statement_string, u ) )
		exit( 1 );
	printf( "%s\n", update_statement_string );

/*
	free_string_list( primary_data_list );
	free_string_list( primary_attribute_list );
	free_string_list( attribute_list );
	free_string_list( data_list );
*/
	return 0;
}
