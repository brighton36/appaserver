// $APPASERVER_HOME/src_predictive/post_change_reoccurring_transaction.js
// ----------------------------------------------------------------------
function post_change_reoccurring_transaction( row )
{
	var element_name;
	var element;
	var disable_transaction_amount = true;
	var disable_bank_upload_search_phrase = true;
	var disable_accrued_daily_amount = true;
	var disable_accrued_monthly_amount = true;
	var transaction_amount_is_slash = false;
	var bank_upload_search_phrase_is_slash = false;
	var accrued_daily_amount_is_slash = false;
	var accrued_monthly_amount_is_slash = false;
	var nothing_is_populated = true;

	if ( row == 0 )
	{
		post_change_reoccurring_transaction_each_row();
		// return true; <-- Need to pass through for insert screen.
	}

	// Turn on edits
	// -------------
	element_name = 'transaction_amount_' + row;
	element = timlib_get_element( element_name );

	if ( element == "" ) return false;

	element.disabled = false;

	element_name = 'bank_upload_search_phrase_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	element_name = 'accrued_daily_amount_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	element_name = 'accrued_monthly_amount_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	// Check transaction_amount
	// ------------------------
	element_name = 'transaction_amount_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" )
	{
		transaction_amount_is_slash = true;
	}
	else
	if ( element.value != "" )
	{
		disable_transaction_amount = false;
		nothing_is_populated = false;
	}

	// Check bank_upload_search_phrase
	// -------------------------------
	element_name = 'bank_upload_search_phrase_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" )
	{
		bank_upload_search_phrase_is_slash = true;
	}
	else
	if ( element.value != "" )
	{
		disable_bank_upload_search_phrase = false;
		nothing_is_populated = false;
	}

	// Check accrued_daily_amount
	// --------------------------
	element_name = 'accrued_daily_amount_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" )
	{
		accrued_daily_amount_is_slash = true;
	}
	else
	if ( element.value != "" )
	{
		disable_accrued_daily_amount = false;
		nothing_is_populated = false;
	}

	// Check accrued_monthly_amount
	// ----------------------------
	element_name = 'accrued_monthly_amount_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" )
	{
		accrued_monthly_amount_is_slash = true;
	}
	else
	if ( element.value != "" )
	{
		disable_accrued_monthly_amount = false;
		nothing_is_populated = false;
	}

	/* Disable widgets */
	/* --------------- */
	if ( disable_transaction_amount
	&&   !transaction_amount_is_slash
	&&   !nothing_is_populated )
	{
		element_name = 'transaction_amount_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}

	if ( disable_bank_upload_search_phrase
	&&   !bank_upload_search_phrase_is_slash
	&&   !nothing_is_populated )
	{
		element_name = 'bank_upload_search_phrase_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}

	if ( disable_accrued_daily_amount
	&&   !accrued_daily_amount_is_slash
	&&   !nothing_is_populated )
	{
		element_name = 'accrued_daily_amount_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}

	if ( disable_accrued_monthly_amount
	&&   !accrued_monthly_amount_is_slash
	&&   !nothing_is_populated )
	{
		element_name = 'accrued_monthly_amount_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
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
