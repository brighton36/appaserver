//
// src_benthic/post_change_sampling_point.js
// -----------------------------------------

function post_change_sampling_point( element )
{
	var value;

	if ( element.name == 'actual_longitude_0' )
	{
		value = element.value;
		if ( value.charAt( 0 ) != '-' )
		{
			alert(
			'Error: actual longitude must begin with a dash.' );
			return falue;
		}
	}

	return true;
} // post_change_sampling_point()

