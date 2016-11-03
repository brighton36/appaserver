//
// src_sparrow/post_change_output_bird_count.js
// --------------------------------------------

function post_change_output_bird_count()
{
	var element;
	var value;

	element = timlib_get_element( "group_by" );
	element.disabled = 0;

	element = timlib_get_element( "quad_sheet" );
	value = timlib_get_drop_down_element_value( element.options );

	if ( value != 'undefined' && value != 'select' )
	{
		element = timlib_get_element( "group_by" );
		element.disabled = 1;
	}

	return true;
}

