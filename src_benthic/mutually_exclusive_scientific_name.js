//
// src_benthic/mutually_exclusive_scientific_name.js
// -------------------------------------------------

function mutually_exclusive_scientific_name( current_element )
{
	var element;
	var value;

	element = timlib_get_element( "family_name^genus_name^species_name" );
	element.disabled = 0;

	element = timlib_get_element( "family_name^genus_name" );
	element.disabled = 0;

	element = timlib_get_element( "family_name" );
	element.disabled = 0;

	element = timlib_get_element( "family_name^genus_name^species_name" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		element = timlib_get_element( "family_name^genus_name" );
		element.disabled = 1;

		element = timlib_get_element( "family_name" );
		element.disabled = 1;

		return true;
	}

	element = timlib_get_element( "family_name^genus_name" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		element =
			timlib_get_element(
				"family_name^genus_name^species_name" );
		element.disabled = 1;

		element = timlib_get_element( "family_name" );
		element.disabled = 1;

		return true;
	}

	element = timlib_get_element( "family_name" );
	value = timlib_get_drop_down_element_value( element.options );
	if ( value != 'undefined' && value != 'select' )
	{
		element =
			timlib_get_element(
				"family_name^genus_name^species_name" );
		element.disabled = 1;

		element = timlib_get_element( "family_name^genus_name" );
		element.disabled = 1;

		return true;
	}

	return true;
}

