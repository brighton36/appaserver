//
// src_creel/calculate_species_cpue.js
// -----------------------------------

function post_change_calculate_species_cpue( current_element )
{
	var summary_only_element;
	var output_medium_element;
	var aggregate_period_element;
	var aggregate_period_value;

	summary_only_element =
		timlib_get_element( "summary_only_yn" );

	summary_only_element.disabled = 0;

	output_medium_element =
		timlib_get_element( "output_medium" );

	output_medium_element.disabled = 0;

	aggregate_period_element =
		timlib_get_element( "aggregate_period" );

	aggregate_period_value =
		timlib_get_drop_down_element_value(
			aggregate_period_element.options );

	if ( aggregate_period_value == "annually"
	||   aggregate_period_value == "monthly" )
	{
		summary_only_element.disabled = 1;
		output_medium_element.disabled = 1;
	}

	return true;
}

