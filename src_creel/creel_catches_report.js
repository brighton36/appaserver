//
// $APPASERVER_HOME/src_creel/creel_catches_report.js
// --------------------------------------------------

function post_change_creel_catches_report( current_element )
{
	var summary_only_element;
	var output_medium_element;
	var output_medium_value;

	summary_only_element =
		timlib_get_element( "from_summary_only_yn_0" );

	summary_only_element.disabled = 0;

	output_medium_element =
		timlib_get_element( "output_medium_0" );

	output_medium_value =
		timlib_get_drop_down_element_value(
			output_medium_element.options );

	if ( output_medium_value == "text_file"
	||   output_medium_value == "stacked_chart" )
	{
		summary_only_element.disabled = 1;
	}

	return true;
}

