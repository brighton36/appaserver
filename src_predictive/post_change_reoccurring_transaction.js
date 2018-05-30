// $APPASERVER_HOME/src_predictive/post_change_reoccurring_transaction.js
// ----------------------------------------------------------------------
function post_change_reoccurring_transaction( row )
{
	var element_name;
	var element;
	var attribute_datatype_value;

	if ( row == 0 )
	{
		post_change_reoccurring_transaction_each_row();
		// return true; <-- Need to pass through for insert screen.
	}

	// Turn on edits
	// -------------
	element_name = 'bank_upload_search_phrase_' + row;
	element = timlib_get_element( element_name );

	if ( element == "" ) return false;

	element.disabled = false;

	element_name = 'accrued_daily_amount_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	element_name = 'accrued_monthly_amount_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	// Check bank_upload_search_phrase
	// -------------------------------
	element_name = 'bank_upload_search_phrase_' + row;
	element = timlib_get_element( element_name );

	if ( element.value != "" )
	{
		element_name = 'accrued_daily_amount_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'accrued_monthly_amount_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
		return true;
	}

	// Check accrued_daily_amount
	// --------------------------
	element_name = 'accrued_daily_amount_' + row;
	element = timlib_get_element( element_name );

	if ( element.value != "" )
	{
		element_name = 'bank_upload_search_phrase_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'accrued_monthly_amount_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
		return true;
	}

	// Check accrued_monthly_amount
	// ----------------------------
	element_name = 'accrued_monthly_amount_' + row;
	element = timlib_get_element( element_name );

	if ( element.value != "" )
	{
		element_name = 'bank_upload_search_phrase_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'accrued_daily_amount_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
		return true;
	}

	return true;

} // post_change_reoccurring_transaction()

function post_change_reoccurring_transaction_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = post_change_reoccurring_transaction( row );
		if ( results == false ) break;
		row++;
	}

	return true;

} // post_change_reoccurring_transaction_each_row()
