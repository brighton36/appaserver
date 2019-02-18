//
// src_waterquality/output_results_process_set.js
// ----------------------------------------------

function post_change_output_results( current_element )
{
	var process_element;
	var element;
	var process_value;
	var aggregate_level_value;

	element =
		timlib_get_element(
			"aggregate_level_0" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"aggregate_statistic_0" );
	element.disabled = 0;

	process_element =
		timlib_get_element(
			"output_results_per_station_0" );

	if ( !process_element )
	{
		process_element =
			timlib_get_element(
				"output_results_per_project_0" );

		if ( !process_element ) return true;
	}

	process_value =
		timlib_get_drop_down_element_value(
			process_element.options );

	if ( !process_value ) return true;

	if ( process_value == 'output_results_high_average_low' )
	{
		element =
			timlib_get_element(
				"aggregate_statistic_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_moving_average' )
	{
		element =
			timlib_get_element(
				"aggregate_level_0" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_statistic_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_results_statistics' )
	{
		element =
			timlib_get_element(
				"aggregate_level_0" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_statistic_0" );
		element.disabled = 1;
	}
	else
	if ( ( process_value == 'output_results_exception' )
	||   ( process_value == 'output_results_exception_free' ) )
	{
		element =
			timlib_get_element(
				"aggregate_level_0" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_statistic_0" );
		element.disabled = 1;
	}

	element =
		timlib_get_element(
			"aggregate_level_0" );

	aggregate_level_value =
		timlib_get_drop_down_element_value(
			element.options );

	if ( aggregate_level_value == undefined
	||   aggregate_level_value == 'undefined'
	||   aggregate_level_value == 'select'
	||   aggregate_level_value == 'real_time'
	||   aggregate_level_value == 'hourly'
	||   aggregate_level_value == 'daily' )
	{
		element =
			timlib_get_element(
				"aggregate_statistic_0" );
		if ( element )
			element.disabled = 1;
	}

	return true;

} // post_change_output_results()

