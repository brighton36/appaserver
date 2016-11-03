//
// src_capitolpops/post_change_program.js
// --------------------------------------

function post_change_program( element, state )
{
	var original_value;
	var value_to_set;
	var upper_element_value;
	var index;
	var element_name;
	var results;
	var upper_element;

	if ( state != 'insert' ) return true;

	element_name = element.name;

	if ( element_name.search( /composition_/ ) == 0 )
	{
		index = timlib_get_index( "composition", element_name );

		if ( index <= 0 ) return true;

		element_name = "sort_order_" + index;
		element = timlib_get_element( element_name );

		if ( !element ) return true;

		original_value = element.value;

		if ( original_value > 0 ) return true;

		value_to_set = index;

		if ( index > 1 )
		{
			element_name = "sort_order_" + ( index - 1 );

			upper_element = timlib_get_element( element_name );

			if ( upper_element )
			{
				upper_element_value = upper_element.value - 0;
				value_to_set = upper_element_value + 1;
			}
		}
		element.value = value_to_set;
	}

	return true;

} // post_change_program()

