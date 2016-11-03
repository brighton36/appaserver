//
// src_modeland/post_change_annual_comparison.js
// ---------------------------------------------

function post_change_annual_comparison( current_element )
{
	var element;
	var element_value;
	var aggregate_level_value;

	element =
		timlib_get_element(
			"aggregate_statistic" );
	element.disabled = 0;

	element =
		timlib_get_element(
			"aggregate_level" );

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
				"aggregate_statistic" );

		element.disabled = 1;
	}
	else
	if ( aggregate_level_value == 'annually' )
	{
		element =
			timlib_get_element(
				"begin_month_day" );

		element.value = "01-01";

		element =
			timlib_get_element(
				"end_month_day" );

		element.value = "12-31";

		element =
			timlib_get_element(
				"output_medium" );

		element_value =
			timlib_get_drop_down_element_value(
				element.options );

		if ( element_value == 'chart' )
		{
			element.value = "select";
			alert(
		'Warning: cannot chart aggregate level of annually.' );
		}

	}

	return true;
} // post_change_annual_comparison()

