/* src_sparrow/sparrow_library.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include "sparrow_library.h"
#include "timlib.h"

char *sparrow_library_get_observation_where(
				char *quad_sheet,
				char *begin_visit_date,
				char *end_visit_date,
				char *table_name )
{
	char where[ 65536 ];

	if ( *quad_sheet && strcmp( quad_sheet, "quad_sheet" ) != 0 )
	{
		sprintf( where,
			 "%s.quad_sheet in (%s)",
			 table_name,
			 timlib_get_in_clause( quad_sheet ) );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}

	if ( begin_visit_date
	&&   strcmp( begin_visit_date, "begin_visit_date" ) != 0
	&&   end_visit_date
	&&   strcmp( end_visit_date, "end_visit_date" ) != 0 )
	{
		if ( strlen( begin_visit_date ) == 4
		&&   strlen( end_visit_date ) == 4 )
		{
			sprintf( where + strlen( where ),
			 " and visit_date between '%s-01-01' and '%s-12-31'",
			 	begin_visit_date,
			 	end_visit_date );
		}
		else
		{
			sprintf( where + strlen( where ),
			 	" and visit_date between '%s' and '%s'",
			 	begin_visit_date,
			 	end_visit_date );
		}
	}

	return strdup( where );

} /* sparrow_library_get_observation_where() */

char *sparrow_library_get_distinct_physical_parameter(
					char *physical_observation,
					char *observation_where )
{
	char sys_string[ 65536 ];

	sprintf( sys_string,
"echo \"select distinct physical_parameter from %s where %s;\" | sql",
		 physical_observation,
		 observation_where );

	return pipe_multiple_lines2string( sys_string, ',' );

} /* sparrow_library_get_distinct_physical_observation() */

char *sparrow_library_get_distinct_vegetation_name(
					char *vegetation_observation,
					char *observation_where )
{
	char sys_string[ 65536 ];

	sprintf( sys_string,
"echo \"select distinct vegetation_name from %s where %s;\" | sql",
		 vegetation_observation,
		 observation_where );

	return pipe_multiple_lines2string( sys_string, ',' );

} /* sparrow_library_get_distinct_physical_observation() */

char *sparrow_library_get_site_id(	char *application_name,
					char *quad_sheet,
					int site_number )
{
	char sys_string[ 1024 ];
	char where[ 512 ];

	sprintf( where,
		 "quad_sheet = '%s' and site_number = %d",
		 quad_sheet,
		 site_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=site_id		"
		 "			folder=observation_site	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* sparrow_library_get_site_id() */

char *sparrow_library_get_title_string(
			char *quad_sheet,
			int site_number,
			char *visit_date )
{
	char static title[ 1024 ];

	sprintf(title,
		"Validate %s/%d, %s",
		quad_sheet,
		site_number,
		visit_date );

	format_initial_capital( title, title );

	return title;
} /* sparrow_library_get_title_string() */

