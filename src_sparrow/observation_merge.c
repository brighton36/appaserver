/* src_sparrow/observation_merge.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "sparrow_library.h"

/* Prototypes */
/* ---------- */
char *get_group_quad_sys_string(	char *quad_sheet_where,
					char *sparrow_observation,
					char *physical_observation,
					char *vegetation_observation );

char *get_group_site_sys_string(	char *quad_sheet_where,
					char *sparrow_observation,
					char *physical_observation,
					char *vegetation_observation );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_visit_date;
	char *end_visit_date;
	char *quad_sheet;
	char group_quad_yn;
	char *sparrow_observation;
	char *physical_observation;
	char *vegetation_observation;
	char *quad_sheet_where;
	char *sys_string;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s ignored begin_visit_date end_visit_date quad_sheet group_quad_yn \n",
			 argv[ 0 ] );
		exit( 1 );
	}

	begin_visit_date = argv[ 2 ];
	end_visit_date = argv[ 3 ];
	quad_sheet = argv[ 4 ];
	group_quad_yn = *argv[ 5 ];

	sparrow_observation =
		get_table_name(
			application_name,
			"sparrow_observation" );

	physical_observation =
		get_table_name(
			application_name,
			"physical_observation" );

	vegetation_observation =
		get_table_name(
			application_name,
			"vegetation_observation" );

	quad_sheet_where =
		sparrow_library_get_observation_where(
			quad_sheet,
			begin_visit_date,
			end_visit_date,
			(char *)0 /* table_name */ );

	if ( group_quad_yn == 'y' )
	{
		sys_string =
			get_group_quad_sys_string(
					quad_sheet_where,
					sparrow_observation,
					physical_observation,
					vegetation_observation );
	}
	else
	{
		sys_string =
			get_group_site_sys_string(
					quad_sheet_where,
					sparrow_observation,
					physical_observation,
					vegetation_observation );
	}

	system( sys_string );

	return 0;

} /* main() */

char *get_group_site_sys_string(	char *quad_sheet_where,
					char *sparrow_observation,
					char *physical_observation,
					char *vegetation_observation )

{
	char sys_string[ 65536 ];
	char *sys_ptr = sys_string;
	char *select;
	char *group_by_clause;

	select =
	"visit_date, quad_sheet, site_number, 'bird_count', sum(bird_count)";
	group_by_clause = "visit_date, quad_sheet, site_number";
	sys_ptr += sprintf(
		sys_ptr,
	 	"(echo \"select %s from %s where %s group by %s;\" | sql '^';",
	 	select,
	 	sparrow_observation,
		quad_sheet_where,
	 	group_by_clause );

	select =
"visit_date,quad_sheet,site_number,physical_parameter,avg(measurement_value)";
	group_by_clause =
		"visit_date, quad_sheet, site_number, physical_parameter";
	sys_ptr += sprintf(
		sys_ptr,
	 	"echo \"select %s from %s where %s group by %s;\" | sql '^';",
	 	select,
	 	physical_observation,
		quad_sheet_where,
	 	group_by_clause );

	select =
"visit_date, quad_sheet, site_number, vegetation_name, predominance_rank";
	sys_ptr += sprintf(
		sys_ptr,
	 	"echo \"select %s from %s where %s;\" | sql '^')",
	 	select,
	 	vegetation_observation,
		quad_sheet_where );

	sys_ptr += sprintf(
		sys_ptr,
		"| sort | pivot_table.e \"bird_count,%s,%s\" '^'",
		sparrow_library_get_distinct_physical_parameter(
			physical_observation,
			quad_sheet_where ),
		sparrow_library_get_distinct_vegetation_name(
			vegetation_observation,
			quad_sheet_where ) );

	return strdup( sys_string );

} /* get_group_site_sys_string() */

char *get_group_quad_sys_string(	char *quad_sheet_where,
					char *sparrow_observation,
					char *physical_observation,
					char *vegetation_observation )

{
	char sys_string[ 65536 ];
	char *sys_ptr = sys_string;
	char *select;
	char *group_by_clause;

	select =
	"visit_date, quad_sheet, 'bird_count', sum(bird_count)";
	group_by_clause = "visit_date, quad_sheet";
	sys_ptr += sprintf(
		sys_ptr,
	 	"(echo \"select %s from %s where %s group by %s;\" | sql '^';",
	 	select,
	 	sparrow_observation,
		quad_sheet_where,
	 	group_by_clause );

	select =
"visit_date,quad_sheet,physical_parameter,avg(measurement_value)";
	group_by_clause =
		"visit_date, quad_sheet, physical_parameter";
	sys_ptr += sprintf(
		sys_ptr,
	 	"echo \"select %s from %s where %s group by %s;\" | sql '^';",
	 	select,
	 	physical_observation,
		quad_sheet_where,
	 	group_by_clause );

	select =
"visit_date,quad_sheet,vegetation_name, avg( predominance_rank )";
	group_by_clause =
		"visit_date, quad_sheet, vegetation_name";
	sys_ptr += sprintf(
		sys_ptr,
	 	"echo \"select %s from %s where %s group by %s;\" | sql '^')",
	 	select,
	 	vegetation_observation,
		quad_sheet_where,
		group_by_clause );

	sys_ptr += sprintf(
		sys_ptr,
		"| sort | pivot_table.e \"bird_count,%s,%s\" '^'",
		sparrow_library_get_distinct_physical_parameter(
			physical_observation,
			quad_sheet_where ),
		sparrow_library_get_distinct_vegetation_name(
			vegetation_observation,
			quad_sheet_where ) );

	return strdup( sys_string );

} /* get_group_quad_sys_string() */

