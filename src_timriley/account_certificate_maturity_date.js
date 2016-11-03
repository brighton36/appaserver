// src_timriley/account_certificate_maturity_date.js
// -------------------------------------------------
function account_certificate_maturity_date( row )
{
	var account_type;
	var account_type_element;
	var certificate_maturity_date;
	var certificate_maturity_date_element;

	if ( row == 0 )
	{
		account_certificate_maturity_date_each_row();
	}

	account_type = 'account_type_' + row;
	account_type_element =
		timlib_get_form_element(
			account_type, 0 );

	if ( account_type_element == "" ) return false;

	certificate_maturity_date = 'certificate_maturity_date_' + row;
	certificate_maturity_date_element =
		timlib_get_form_element(
			certificate_maturity_date, 0 );

	if ( account_type_element.value != 'certificate' )
		certificate_maturity_date_element.hidden = true;
	else
		certificate_maturity_date_element.hidden = false;

	return true;
} // account_certificate_maturity_date()

function account_certificate_maturity_date_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = account_certificate_maturity_date( row );
		if ( results == false ) break;
		row++;
	}

	return true;
} // account_certificate_maturity_date_each_row()
