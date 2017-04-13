//
// src_benthic/output_scatterplot.js
// ---------------------------------

function post_change_output_scatterplot( current_element )
{
	var element;
	var value;
	var disable_vegetation_habitat = 1;

	element = timlib_get_element( "family_name^genus_name^species_name_0" );
	element.disabled = 0;

	element = timlib_get_element( "vegetation_name_0" );
	element.disabled = 0;

	element = timlib_get_element( "datatype_0" );
	element.disabled = 0;

	element = timlib_get_element( "vegetation_habitat_0" );
	element.disabled = 0;

	element = timlib_get_element( "substrate_0" );
	element.disabled = 0;

	element = timlib_get_element( "counts_measurements_0" );
	element.disabled = 0;

	// If scientific_name and vegetation_name are selected.
	// ----------------------------------------------------
	element = timlib_get_element( "family_name^genus_name^species_name_0" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		element = timlib_get_element( "vegetation_name_0" );
		value = timlib_get_drop_down_element_value( element.options );
		if ( value != 'undefined' && value != 'select' )
		{
			element = timlib_get_element( "datatype_0" );
			element.disabled = 1;

			element = timlib_get_element( "substrate_0" );
			element.disabled = 1;

			disable_vegetation_habitat = 0;

			return true;
		}
	}

	// If scientific_name and datatype are selected.
	// ---------------------------------------------
	element = timlib_get_element( "family_name^genus_name^species_name_0" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		element = timlib_get_element( "datatype_0" );
		value = timlib_get_drop_down_element_value( element.options );
		if ( value != 'undefined' && value != 'select' )
		{
			element = timlib_get_element( "vegetation_name_0" );
			element.disabled = 1;

			element = timlib_get_element( "vegetation_habitat_0" );
			element.disabled = 1;

			element = timlib_get_element( "substrate_0" );
			element.disabled = 1;

			return true;
		}
	}

	// If scientific_name is not selected and vegetation_name is.
	// ----------------------------------------------------------
	element = timlib_get_element( "family_name^genus_name^species_name_0" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value == 'undefined' || value == 'select' )
	{
		element = timlib_get_element( "vegetation_name_0" );
		value = timlib_get_drop_down_element_value( element.options );
		if ( value != 'undefined' && value != 'select' )
		{
			element = timlib_get_element( "substrate_0" );
			element.disabled = 1;

			element = timlib_get_element( "counts_measurements_0" );
			element.disabled = 1;
		}
	}

	// If vegetation and datatype are selected
	// ---------------------------------------
	element = timlib_get_element( "vegetation_name_0" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		element = timlib_get_element( "datatype_0" );
		value = timlib_get_drop_down_element_value( element.options );
		if ( value != 'undefined' && value != 'select' )
		{
			element =
				timlib_get_element(
				"family_name^genus_name^species_name_0" );
			element.disabled = 1;

			element = timlib_get_element( "substrate_0" );
			element.disabled = 1;

			element = timlib_get_element( "counts_measurements_0" );
			element.disabled = 1;

			return true;
		}
	}

	if ( disable_vegetation_habitat )
	{
		element = timlib_get_element( "vegetation_habitat_0" );
		element.disabled = 1;
	}

	return true;
}

