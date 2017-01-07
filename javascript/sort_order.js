// $DOCUMENT_ROOT/javascript/sort_order.js
// ---------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function sort_order_set_all_selected()
{
	var element;
	var i;

	element = timlib_get_element( "sort_order_1" )

	for( i = 0; i < element.options.length; i++ )
	{
		element.options[ i ].selected = true;
	}
	return true;
}

function sort_order_is_multi_selected( element )
{
	var i;
	var selected_count;

	selected_count = 0;

	for( i = 0; i < element.options.length; i++ )
	{
		if ( element.options[ i ].selected )
		{
			selected_count++;
		}
	}
	if ( selected_count > 1 )
		return true;
	else
		return false;
}

function sort_order_move_up_move( element, i )
{
	var value;
	var value_above;

	value_above = element.options[ i - 1 ].value;
	value = element.options[ i ].value;

	element.options[ i - 1 ] = new Option( value, value, 0, 1 );
	element.options[ i ] = new Option( value_above, value_above, 0, 0 );
	return true;
}

function sort_order_move_up()
{
	var element;
	var i;

	element = timlib_get_element( "sort_order_1" )

	if ( sort_order_is_multi_selected( element ) )
	{
		alert( 'Only select one, please.' );
		return false;
	}

	for( i = 1; i < element.options.length; i++ )
	{
		if ( element.options[ i ].selected )
		{
			sort_order_move_up_move( element, i );
			i++;
		}
	}
	return true;
}

function sort_order_move_down_move( element, i )
{
	var value;
	var value_below;

	value = element.options[ i ].value;
	value_below = element.options[ i + 1 ].value;

	element.options[ i ] = new Option( value_below, value_below, 0, 0 );
	element.options[ i + 1 ] = new Option( value, value, 0, 1 );
	return true;
}

function sort_order_move_down()
{
	var element;
	var i;

	element = timlib_get_element( "sort_order_1" )

	if ( sort_order_is_multi_selected( element ) )
	{
		alert( 'Only select one, please.' );
		return false;
	}

	for( i = 0; i < element.options.length - 1; i++ )
	{
		if ( element.options[ i ].selected )
		{
			sort_order_move_down_move( element, i );
			i++;
		}
	}
	return true;
}
