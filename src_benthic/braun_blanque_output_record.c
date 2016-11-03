void braun_blanque_output_record(	FILE *output_file,
					OUTPUT_RECORD *output_record,
					int length_vegetation_list,
					int length_vegetation_group_list,
					VEGETATION **vegetation_array )
{
	VALUE *value;
	VALUE **value_array;
	VEGETATION_GROUP_VALUE **group_value_array;
	VEGETATION_GROUP_VALUE *vegetation_group_value;
	int i;
	char max_height_string[ 16 ];
	char average_height_string[ 16 ];

	value_array = output_record->value_array;
	group_value_array = output_record->group_value_array;

	if ( output_record->max_height )
	{
		sprintf(	max_height_string,
				"%.1lf",
				output_record->max_height );
	}
	else
	{
		*max_height_string = '\0';
	}

	if ( output_record->average_height )
	{
		sprintf(	average_height_string,
				"%.1lf",
				output_record->average_height );
	}
	else
	{
		*average_height_string = '\0';
	}

	fprintf( output_file,
		 "%d,%d,%s,%s,%s,%s,%s,%d,%d,%d,%d,%s,%s,%s",
		 output_record->collection_number,
		 output_record->location_number,
		 output_record->collection_name,
		 output_record->location,
		 output_record->actual_latitude,
		 output_record->actual_longitude,
		 output_record->region_code,
		 output_record->pink_assessment_area,
		 output_record->site_number,
		 output_record->quad,
		 output_record->season,
		 braun_blanque_get_substrate_display(
				output_record->substrate_list ),
		 max_height_string,
		 average_height_string );

	for( i = 0; i < length_vegetation_group_list; i++ )
	{
		vegetation_group_value = group_value_array[ i ];

		if ( vegetation_group_value )
		{
			if ( !vegetation_group_value->count )
			{
				fprintf( output_file, ",0" );
			}
			else
			{
				fprintf(output_file,
					",%.1lf",
					vegetation_group_value->value_sum /
					(double)
					vegetation_group_value->count );
			}
		}
		else
		{
			fprintf( output_file, ",0" );
		}
	}

	for( i = 0; i < length_vegetation_list; i++ )
	{
		value = value_array[ i ];

		if ( value )
		{
			fprintf( output_file, ",%.1lf", value->value );
		}
		else
		{
			if ( timlib_strcmp(	vegetation_array[ i ]->
							vegetation_group,
						"seagrass" ) == 0
			&&   vegetation_array[ i ]->is_height_yn == 'y' )
			{
				fprintf( output_file, ",M" );
			}
			else
			{
				fprintf( output_file, ",0" );
			}
		}
	}

	fprintf( output_file, "\n" );

} /* braun_blanque_output_record() */

