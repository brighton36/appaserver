// linear_interpolation_process_set.js
// ------------------------------------

function post_change_linear_interpolation( current_element )
{
	var process_element;
	var element;
	var process_value;
	var value;

	element =
		timlib_get_element(
			"from_first_value_0" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"from_last_value_0" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"from_first_multiplier_0" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"from_last_multiplier_0" );
	element.disabled = 0;

	process_element =
		timlib_get_element( "estimation_linear_interpolation_0" );

	process_value =
		timlib_get_drop_down_element_value(
			process_element.options );

	if ( process_value == 'linear_interpolation_drift_by_ratio' )
	{
		element =
			timlib_get_element(
				"from_first_multiplier_0" );
		element.disabled = 1;

		element =
			timlib_get_element(
				"from_last_multiplier_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'linear_interpolation_drift' )
	{
		element =
			timlib_get_element(
				"from_first_value_0" );

		value = element.value;

		if ( value != undefined
		&&   value != 'undefined'
		&&   value != '' )
		{
			element =
				timlib_get_element(
					"from_first_multiplier_0" );
			element.disabled = 1;

			element =
				timlib_get_element(
					"from_last_multiplier_0" );
			element.disabled = 1;

		}

		element =
			timlib_get_element(
				"from_first_multiplier_0" );

		value = element.value;

		if ( value != undefined
		&&   value != 'undefined'
		&&   value != '' )
		{
			element =
				timlib_get_element(
					"from_first_value_0" );
			element.disabled = 1;

			element =
				timlib_get_element(
					"from_last_value_0" );
			element.disabled = 1;
		}
	}
	else
	if ( process_value == 'linear_interpolation_straight_line' )
	{
		element =
			timlib_get_element(
				"from_first_multiplier_0" );
		element.disabled = 1;

		element =
			timlib_get_element(
				"from_last_multiplier_0" );
		element.disabled = 1;
	}
	return true;
} // post_change_linear_interpolation()

