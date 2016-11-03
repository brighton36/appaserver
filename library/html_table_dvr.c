/* html_table_dvr.c		         */
/* ------------------------------------- */
/* Test drive program for HTML_TABLE ADT */
/* ------------------------------------- */
/* Tim Riley				 */
/* ------------------------------------- */

#include <stdio.h>
#include "document.h"
#include "application.h"
#include "html_table.h"
#include "appaserver_parameter_file.h"

#define APPLICATION	"everglades"
#define TITLE		"Bear sightings in Western Massachusetts"

int main( void )
{
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	HTML_TABLE *t = new_html_table(
			TITLE,
			(char *)0 /* sub_title */ );

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( TITLE, APPLICATION );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			(char *)0 /* relative_source_directory */,
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );


	html_table_set_heading( t, "City" );
	html_table_set_heading( t, "Babies" );
	html_table_set_heading( t, "Adults" );
	html_table_set_heading( t, "Total" );

	html_table_output_table_heading(t->title,
					t->sub_title );
	html_table_output_data_heading(  t->heading_list,
					 t->number_left_justified_columns,
					 t->number_right_justified_columns,
					 t->justify_list );

	t->data_list = list_new();
	html_table_set_data( t->data_list, "Northampton" );
	html_table_set_data( t->data_list, "2" );
	html_table_set_data( t->data_list, "4" );
	html_table_set_data( t->data_list, "6" );
	html_table_output_data( t->data_list,
				t->number_left_justified_columns,
				t->number_right_justified_columns,
				t->background_shaded,
				t->justify_list );

	t->data_list = list_new();
	html_table_set_data( t->data_list, "Becket" );
	html_table_set_data( t->data_list, "5" );
	html_table_set_data( t->data_list, "22" );
	html_table_set_data( t->data_list, "27" );
	html_table_output_data( t->data_list,
				t->number_left_justified_columns,
				t->number_right_justified_columns,
				t->background_shaded,
				t->justify_list );

	t->data_list = list_new();
	html_table_set_data( t->data_list, "Worthington" );
	html_table_set_data( t->data_list, "7" );
	html_table_set_data( t->data_list, "5" );
	html_table_set_data( t->data_list, "12" );
	html_table_output_data( t->data_list,
				t->number_left_justified_columns,
				t->number_right_justified_columns,
				t->background_shaded,
				t->justify_list );

	html_table_close();
	document_close();
	return 0;
}

