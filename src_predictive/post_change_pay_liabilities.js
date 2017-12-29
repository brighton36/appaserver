// src_accountancymodel/post_change_pay_liabilities.js
// ---------------------------------------------------

function post_change_pay_liabilities( current_element )
{
	var element;
	var value;

	element = timlib_get_element( "memo" );
	element.disabled = 1;

	element = timlib_get_element( "with_stub_yn" );
	element.disabled = 1;

	element = timlib_get_element( "starting_check_number" );
	value = element.value;

	if ( value >= 1 )
	{
		element = timlib_get_element( "memo" );
		element.disabled = 0;

		element = timlib_get_element( "with_stub_yn" );
		element.disabled = 0;
	}

	return true;

} // post_change_pay_liabilities()

