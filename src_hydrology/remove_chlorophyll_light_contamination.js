//
// remove_chlorophyll_light_contamination.js
// -----------------------------------------

function post_change_remove_chlorophyll( current_element )
{
	var element;

	// Turn off
	// --------
	element =
		timlib_get_element(
			"begin_time" );
	element.disabled = 0;

	element =
		timlib_get_element(
			"end_time" );
	element.disabled = 0;

	element =
		timlib_get_element(
			"contamination_seek_yn" );
	element.disabled = 0;

	element =
		timlib_get_element(
			"chart_yn" );
	element.disabled = 0;

	element =
		timlib_get_element(
			"execute_yn" );
	element.disabled = 0;

	// Perform checks
	// --------------
	element =
		timlib_get_element(
			"execute_yn" );
	if ( element.value == 'y' )
	{
		element =
			timlib_get_element(
				"chart_yn" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"contamination_seek_yn" );
		element.disabled = 1;
		return true;
	}

	element =
		timlib_get_element(
			"contamination_seek_yn" );
	if ( element.value == 'y' )
	{
		element =
			timlib_get_element(
				"begin_time" );
		element.disabled = 1;

		element =
			timlib_get_element(
				"end_time" );
		element.disabled = 1;

		element =
			timlib_get_element(
				"chart_yn" );
		element.disabled = 1;
		return true;
	}

	element =
		timlib_get_element(
			"chart_yn" );
	if ( element.value == 'y' )
	{
		element =
			timlib_get_element(
				"begin_time" );
		element.disabled = 1;

		element =
			timlib_get_element(
				"end_time" );
		element.disabled = 1;

		element =
			timlib_get_element(
				"contamination_seek_yn" );
		element.disabled = 1;
		return true;
	}

	return true;

} // post_change_remove_chlorophyll()

