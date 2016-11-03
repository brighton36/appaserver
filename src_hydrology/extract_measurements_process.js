//
// extract_measurements_process.js
// -----------------------------------

function post_change_extract_measurements( current_element )
{
	var element;
	var aggregate_level_value;

	element =
		timlib_get_element(
			"aggregate_statistic_0" );
	element.disabled = 0;

	element =
		timlib_get_element(
			"aggregate_level_0" );

	aggregate_level_value =
		timlib_get_drop_down_element_value(
			element.options );

	if ( aggregate_level_value == undefined
	||   aggregate_level_value == 'undefined'
	||   aggregate_level_value == 'select'
	||   aggregate_level_value == 'real_time' )
	{
		element =
			timlib_get_element(
				"aggregate_statistic_0" );
		if ( element )
			element.disabled = 1;
	}

	element =
		timlib_get_element(
			"combined_file_yn" );

	if ( element.value == 'y' )
	{
		element =
			timlib_get_element(
				"aggregate_statistic" );
		if ( element )
			element.disabled = 1;
	}

	return true;
} // post_change_extract_measurements()

