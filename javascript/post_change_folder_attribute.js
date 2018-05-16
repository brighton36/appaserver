// $APPASERVER_HOME/javascript/post_change_folder_attribute.js
// -----------------------------------------------------------
function post_change_folder_attribute( row )
{
	var element_name;
	var element;
	var primary_key_element_value;
	var display_order_element_value;

	if ( row == 0 )
	{
		post_change_folder_attribute_each_row();
		// return true; <-- Need to pass through for insert screen.
	}

	// Turn on each edit.
	// ------------------
	element_name = 'primary_key_index_' + row;
	element = timlib_get_element( element_name );

	if ( element == "" ) return false;

	element.disabled = false;

	element_name = 'display_order_' + row;
	element =
		timlib_get_element( element_name );

	if ( element == "" ) return false;

	element.disabled = false;

	element_name = 'primary_key_index_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" ) return true;
	primary_key_element_value = Number( element.value );

	element_name = 'display_order_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" ) return true;
	display_order_element_value = Number( element.value );

	// Turn off one of them
	// --------------------
	if ( primary_key_element_value )
	{
		element_name = 'display_order_' + row;
		element = timlib_get_element( element_name );

		element.disabled = true;
	}
	else
	if ( display_order_element_value )
	{
		element_name = 'primary_key_index_' + row;
		element = timlib_get_element( element_name );

		element.disabled = true;
	}

	return true;

} // post_change_folder_attribute()

function post_change_folder_attribute_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = post_change_folder_attribute( row );
		if ( results == false ) break;
		row++;
	}

	return true;

} // post_change_folder_attribute_each_row()
