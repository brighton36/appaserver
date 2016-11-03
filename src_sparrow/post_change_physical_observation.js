// src_sparrow/post_change_physical_observation.js
// -------------------------------------------------
function post_change_physical_observation( row, state )
{
	var element;
	var element_name;

	if ( state != "insert" ) return true;
	if ( row != 0 ) return true;

	for( row = 1; row <= 6; row++ )
	{
		element_name = 'physical_parameter_' + row;

		element = 
			timlib_get_form_element(
				element_name, 0 );

		if ( element == "" ) return false;

		timlib_set_drop_down_element_value(
			element.options, "water_depth" );

		element_name = 'measurement_number_' + row;

		element = 
			timlib_get_form_element(
				element_name, 0 );

		if ( element == "" ) return false;

		element.value = row;

		element_name = 'measurement_value_' + row;

		element = 
			timlib_get_form_element(
				element_name, 0 );

		if ( element == "" ) return false;

		element.value = 0;

	}

	return true;

} // post_change_physical_observation()

