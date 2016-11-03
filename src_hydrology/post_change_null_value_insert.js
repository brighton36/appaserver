// src_hydrology/post_change_null_value_insert.js
// -----------------------------------------------

function post_change_null_value_insert( element )
{
	// If screen just loaded
	// ---------------------
	if ( element.name == "prompt_frame" )
	{
		var element;

		element =
			timlib_get_element(
				"from_delete_yn_0" );
		element.value = "n";
		return true;
	}

	if ( element.name.substr( -7 ) == "_time_0" )
	{
		return validate_time( element);
	}

	return true;
} // post_change_null_value_insert()

