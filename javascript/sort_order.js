// $DOCUMENT_ROOT/javascript/sort_order.js
// ---------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function sort_order_get_index( value )
{
	var index_string;
	var underbar_position;

	underbar_position = value.indexOf( '_' );

	if ( underbar_position == undefined )
	{
		return "";
	}

	index_string = value.substr( underbar_position + 1 );
	return index_string;

} // sort_order_get_index()

function sort_order_move_attribute_up_shift_down(
					move_index,
					here_index,
					attribute_name )
{
	var hold_data;
	var shift_data;
	var element_name;
	var element;
	var move_index_integer;
	var here_index_integer;
	var i;

	element_name = attribute_name + '_' + move_index;
	element = timlib_get_form_element( element_name, 0 );
	hold_data = element.value;

	move_index_integer = move_index - 0;
	here_index_integer = here_index - 0;

	for(	i = move_index_integer - 1;
		i >= here_index_integer;
		i-- )
	{
		element_name = attribute_name + '_' + i;
		element = timlib_get_form_element( element_name, 0 );
		shift_data = element.value;

		element_name = attribute_name + '_' + ( i + 1 );
		element = timlib_get_form_element( element_name, 0 );
		element.value = shift_data;
	}

	element_name = attribute_name + '_' + here_index;
	element = timlib_get_form_element( element_name, 0 );
	element.value = hold_data;

	return true;

} // sort_order_move_attribute_up_shift_down()

function sort_order_move_attribute_down_shift_up(
					move_index,
					here_index,
					attribute_name )
{
	var hold_data;
	var shift_data;
	var element_name;
	var element;
	var move_index_integer;
	var here_index_integer;
	var i;

	element_name = attribute_name + '_' + move_index;
	element = timlib_get_form_element( element_name, 0 );
	hold_data = element.value;

	move_index_integer = move_index - 0;
	here_index_integer = here_index - 0;

	for(	i = move_index_integer + 1;
		i <= here_index_integer;
		i++ )
	{
		element_name = attribute_name + '_' + i;
		element = timlib_get_form_element( element_name, 0 );
		shift_data = element.value;

		element_name = attribute_name + '_' + ( i - 1 );
		element = timlib_get_form_element( element_name, 0 );
		element.value = shift_data;
	}

	element_name = attribute_name + '_' + here_index;
	element = timlib_get_form_element( element_name, 0 );
	element.value = hold_data;

	return true;

} // sort_order_move_attribute_down_shift_up()

function sort_order_move_down_shift_up(
				move_index,
				here_index,
				attribute_list_string )
{
	var attribute_name_list;
	var attribute_name;
	var i;

	attribute_name_list =
		attribute_list_string.split( ',' );

	for( i = 0; i < attribute_name_list.length; i++ )
	{
		attribute_name = attribute_name_list[ i ];

		sort_order_move_attribute_down_shift_up(
			move_index,
			here_index,
			attribute_name );
	}

	return true;

} // sort_order_move_down_shift_up()

function sort_order_move_up_shift_down(
				move_index,
				here_index,
				attribute_list_string )
{
	var attribute_name_list;
	var attribute_name;
	var i;

	attribute_name_list =
		attribute_list_string.split( ',' );

	for( i = 0; i < attribute_name_list.length; i++ )
	{
		attribute_name = attribute_name_list[ i ];

		sort_order_move_attribute_up_shift_down(
			move_index,
			here_index,
			attribute_name );
	}

	return true;

} // sort_order_move_up_shift_down()

function sort_order_move( attribute_list_string )
{
	var move_value;
	var move_index;
	var move_index_integer;
	var here_value;
	var here_index;
	var here_index_integer;

	move_value = timlib_get_radio_selected_value( "move" );

	if ( move_value == "" ) return true;

	move_index = sort_order_get_index( move_value );

	move_index_integer = move_index - 0;

	here_value = timlib_get_radio_selected_value( "here" );

	here_index = sort_order_get_index( here_value );

	here_index_integer = here_index - 0;

	if ( move_index_integer == here_index_integer )
	{
		return true;
	}
	else
	if ( move_index_integer > here_index_integer )
	{
		sort_order_move_up_shift_down(
			move_index,
			here_index,
			attribute_list_string );
	}
	else
	{
		sort_order_move_down_shift_up(
			move_index,
			here_index,
			attribute_list_string );
	}

	return true;

} // sort_order_move()

function sort_order_renumber( sort_attribute_name )
{
	var element_name;
	var element;
	var i;

	for(	i = 1;
		1 == 1;
		i++ )
	{
		element_name = sort_attribute_name + '_' + i;
		element = timlib_get_form_element( element_name, 0 );

		if ( element == "" ) break;

		element.value = i;
	}

} // sort_order_renumber()

