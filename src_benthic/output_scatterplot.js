//
// src_benthic/output_scatterplot.js
// ---------------------------------

function post_change_output_scatterplot( current_element )
{
	var element;
	var value;

	element = timlib_get_element( "family_name^genus_name^species_name_0" );
	element.disabled = 0;

	element = timlib_get_element( "vegetation_name_0" );
	element.disabled = 0;

	element = timlib_get_element( "datatype_0" );
	element.disabled = 0;

	element = timlib_get_element( "vegetation_habitat_0" );
	element.disabled = 1;

	element = timlib_get_element( "substrate_0" );
	element.disabled = 0;

	element = timlib_get_element( "counts_measurements_0" );
	element.disabled = 0;

	// If scientific_name and vegetation_name are selected
	// then disable everything else, except counts_measurements.
	// ---------------------------------------------------------
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

			element = timlib_get_element( "vegetation_habitat_0" );
			element.disabled = 1;

			element = timlib_get_element( "substrate_0" );
			element.disabled = 1;

			return true;
		}
	}

	// If scientific_name and datatype are selected
	// then disable everything else, except counts_measurements.
	// ---------------------------------------------------------
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

	// If scientific_name is not selected and vegetation_name is,
	// then enable datatype only.
	// --------------------------------------------------------
	element = timlib_get_element( "family_name^genus_name^species_name_0" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value == 'undefined' || value == 'select' )
	{
		element = timlib_get_element( "vegetation_name_0" );
		value = timlib_get_drop_down_element_value( element.options );
		if ( value != 'undefined' && value != 'select' )
		{
			element = timlib_get_element( "vegetation_habitat_0" );
			element.disabled = 1;

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

			element = timlib_get_element( "vegetation_habitat_0" );
			element.disabled = 1;

			element = timlib_get_element( "substrate_0" );
			element.disabled = 1;

			element = timlib_get_element( "counts_measurements_0" );
			element.disabled = 1;

			return true;
		}
	}

	// If vegetation is selected.
	// --------------------------
	element = timlib_get_element( "vegetation_name_0" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		// element = timlib_get_element( "datatype_0" );
		// element.disabled = 1;
		element = timlib_get_element( "substrate_0" );
		element.disabled = 1;
		element = timlib_get_element( "vegetation_habitat_0" );
		element.disabled = 0;
	}

	// If datatype is selected.
	// ------------------------
	element = timlib_get_element( "datatype_0" );
	value = timlib_get_drop_down_element_value( element.options );

	if ( value != 'undefined' && value != 'select' )
	{
		// element = timlib_get_element( "vegetation_name_0" );
		// element.disabled = 1;
		element = timlib_get_element( "substrate_0" );
		element.disabled = 1;
	}

	// If substrate is selected.
	// -------------------------
	element = timlib_get_element( "substrate_0" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		element = timlib_get_element( "datatype_0" );
		element.disabled = 1;
		element = timlib_get_element( "vegetation_name_0" );
		element.disabled = 1;
	}

	return true;
}

