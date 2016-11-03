//
// null2slash.js
// ---------------------------------------------------------------

function null2slash( element )
{
	var regular_expression = "password_[0-9][0-9]*$";
	var results;

	if ( left_trim( element.value ) == "" ) element.value = "/";

	results = element.name.match( regular_expression );

	if ( results != null && element.value == '/' )
	{
		alert( 'Warning: this account will be locked.' );
	}
	return true;
}

function password_compare( password, password_compare )
{
	var password_element;
	var password_compare_element;

	password_element = timlib_get_form_element( password, 0 );

	password_compare_element =
		timlib_get_form_element(
			password_compare, 0 );

	if ( password_element.value != '/'
	&&   password_element.value != password_compare_element.value )
	{
		alert( 'Warning: the passwords do not match.' );
		return false;
	}

	return true;
}
