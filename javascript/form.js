// javascript/form.js
// ------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function form_reset( form, index_delimiter )
{
	var i;
	var to_element_name;
	var original_element_name;
	var element;
	var value;

	for( i = 0; i < form.elements.length; i++ )
	{
		element = form.elements[ i ];

		if ( element.type == 'select-one'
		||   element.type == 'select-multiple'
		||   element.type == 'text' )
		{
			element.disabled = 0;
		}

		// ---------------------------------------------
		// Find the single selects.
		// Note: form.reset() doesn't work on Konqueror.
		// ---------------------------------------------
		if ( element.type == 'select-one' )
		{
			value =
				timlib_get_drop_down_element_value(
					element.options );

			if ( value != 'select' )
			{
				timlib_set_drop_down_element_value(
					element.options,
					'select' );
			}
		}

		// Find the multi-selects
		// ----------------------
		original_element_name = element.name;
		if ( element.type == 'select-multiple'
		&&   original_element_name.substring( 0, 9 ) == "original_" )
		{
			to_element_name = original_element_name.substring( 9 );
			post_change_multi_select_all( to_element_name );
			post_change_multi_select_move_left(
				original_element_name,
				to_element_name,
				index_delimiter );
		}
	}
	form.reset();
} // form_reset()

function form_push_button_set_all(	push_button_name,
					form_number )
{
	var set_all_element;
	var set_all_element_name;
	var push_button_element_name;
	var push_button_element;
	var row;

	set_all_element_name = 'push_button_set_all_' + push_button_name;
	set_all_element =
		timlib_get_form_element(
			set_all_element_name, form_number );

	for( row = 1; ; row++ )
	{
		push_button_element_name = push_button_name + "_" + row;

		push_button_element =
			timlib_get_form_element(
				push_button_element_name,
				form_number );

		if ( !push_button_element ) break;

		push_button_element.checked = set_all_element.checked;
	}

	return 1;
} // form_push_button_set_all()

