// src_predictive/post_change_fixed_asset_purchase.js
// --------------------------------------------------------
function post_change_fixed_asset_purchase( row )
{
	var element_name;
	var element;

	if ( row == 0 )
	{
		post_change_fixed_asset_purchase_each_row();
	}

	// Turn on each edit.
	// ------------------
	element_name = 'expense_account_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = false;

	element_name = 'estimated_useful_life_years_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = false;

	element_name = 'estimated_useful_life_units_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = false;

	element_name = 'estimated_residual_value_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = false;

	element_name = 'declining_balance_n_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = false;

	element_name = 'depreciation_method_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = false;

	// Check Expense Account element
	// -----------------------------
	element_name = 'expense_account_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );

	if ( element == "" ) return false;

	if ( element.value == 'select'
	||   element.value == 'is_empty' )
	{
		return post_change_fixed_asset_purchase_depreciation( row )
	}
	else
	{
		return post_change_fixed_asset_purchase_expense( row )
	}
  
} // post_change_fixed_asset_purchase()

function post_change_fixed_asset_purchase_expense( row )
{
	var element_name;
	var element;

	// Turn off each edit.
	// -------------------
	element_name = 'estimated_useful_life_years_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = true;

	element_name = 'estimated_useful_life_units_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = true;

	element_name = 'estimated_residual_value_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = true;

	element_name = 'declining_balance_n_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = true;

	element_name = 'depreciation_method_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = true;

	return true;

} // post_change_fixed_asset_purchase_expense()

function post_change_fixed_asset_purchase_depreciation( row )
{
	var element_name;
	var element;
	var depreciation_method_element_value;

	element_name = 'depreciation_method_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	depreciation_method_element_value = element.value;

	if ( depreciation_method_element_value == 'select'
	||   depreciation_method_element_value == 'is_empty' )
	{
		return true;
	}

	element_name = 'expense_account_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = true;

	if ( depreciation_method_element_value == 'straight_line' )
	{
		element_name = 'estimated_useful_life_units_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'double_declining_balance' )
	{
		element_name = 'estimated_useful_life_units_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'n_declining_balance' )
	{
		element_name = 'estimated_useful_life_units_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'sum_of_years_digits' )
	{
		element_name = 'estimated_useful_life_units_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'units_of_production' )
	{
		element_name = 'estimated_useful_life_years_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = true;
	}

	return true;

} // post_change_fixed_asset_purchase_depreciation()

function post_change_fixed_asset_purchase_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = post_change_fixed_asset_purchase( row );
		if ( results == false ) break;
		row++;
	}

	return true;
} // post_change_fixed_asset_purchase_each_row()
