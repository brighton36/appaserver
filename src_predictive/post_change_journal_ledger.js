// src_predictive/post_change_journal_ledger.js
// --------------------------------------------------

function post_change_journal_ledger( state )
{
	var element_name;
	var debit_element;
	var credit_element;
	var i;
	var first_empty_row;
	var total;

	if ( state != 'insert' ) return true;

	total = 0.0;
	first_empty_row = 0;
	i = 1;

	while( true )
	{
		element_name = 'debit_amount_' + i;
		debit_element = timlib_get_element( element_name );

		if ( debit_element == '' ) break;

		element_name = 'credit_amount_' + i;
		credit_element = timlib_get_element( element_name );

		if ( debit_element == '' ) break;

		if ( debit_element.value == 0.0
		&&   credit_element.value == 0.0 )
		{
			return true;
		}

		if ( debit_element.value )
		{
			total += debit_element.value;
		}
		else
		{
			if ( credit_element.value )
			{
				total -= credit_element.value;
			}
			else
			{
				first_empty_row = i;
				break;
			}
		}

		i++;
	}

	if ( !first_empty_row ) return true;

	// Why is there a leading zero?
	// ----------------------------
	if ( total.charAt( 0 ) == '0' ) total = total.substr( 1 );

	if ( total > 0 )
	{
		element_name = 'credit_amount_' + first_empty_row;
		credit_element = timlib_get_element( element_name );
		credit_element.value = total;
	}
	else
	if ( total < 0 )
	{
		element_name = 'debit_amount_' + first_empty_row;
		debit_element = timlib_get_element( element_name );
		debit_element.value = 0 - total;
	}

	return true;

} // post_change_journal_ledger()

