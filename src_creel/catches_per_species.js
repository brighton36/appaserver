//
// $APPASERVER_HOME/src_creel/catches_per_species.js
// -------------------------------------------------

function post_change_catches_per_species( current_element )
{
	var families_element;
	var families_value;
	var genera_element;
	var genera_value;
	var species_element;
	var species_value;

	families_element =
		timlib_get_element( "family_0" );
	families_value =
		timlib_get_drop_down_element_value(
			families_element.options );

	genera_element =
		timlib_get_element( "family^genus_0" );
	genera_value =
		timlib_get_drop_down_element_value(
			genera_element.options );

	species_element =
		timlib_get_element( "family^genus^species_0" );
	species_value =
		timlib_get_drop_down_element_value(
			species_element.options );

	if ( families_value != "select" )
	{
		genera_element.disabled = 1;
		species_element.disabled = 1;
		return true;
	}
	else
	{
		genera_element.disabled = 0;
		species_element.disabled = 0;
	}

	if ( genera_value != "select" )
	{
		families_element.disabled = 1;
		species_element.disabled = 1;
		return true;
	}
	else
	{
		families_element.disabled = 0;
		species_element.disabled = 0;
	}

	if ( species_value != "select" )
	{
		families_element.disabled = 1;
		genera_element.disabled = 1;
		return true;
	}
	else
	{
		families_element.disabled = 0;
		genera_element.disabled = 0;
	}

	return true;

} // post_change_catches_per_species()

