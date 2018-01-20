// src_cesi/post_change_project.js
// -------------------------------
function post_change_project( row )
{
	var element_name;
	var element;
	var value;

	if ( row == 0 )
	{
		post_change_project_each_row();
		// return true; <-- Need to pass through for insert screen.
	}

	// Turn off each edit.
	// -------------------
	element_name = 'master_cooperative_agreement_start_date_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );

	if ( element == "" ) return false;

	element.disabled = true;

	element_name = 'master_cooperative_agreement_end_date_' + row;
	element =
		timlib_get_form_element(
			element_name, 0 );
	element.disabled = true;

	element_name = 'award_type_' + row;

	element = timlib_get_element( element_name );

	value = timlib_get_drop_down_element_value( element.options );

	if ( value == 'task' )
	{
		// Turn on each edit.
		// ------------------
		element_name = 'master_cooperative_agreement_start_date_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = false;

		element_name = 'master_cooperative_agreement_end_date_' + row;
		element =
			timlib_get_form_element(
				element_name, 0 );
		element.disabled = false;
	}

	return true;

} // post_change_project()

function post_change_project_each_row()
{
	var row;
	var results;

	row = 1;

	while( true )
	{
		results = post_change_project( row );
		if ( results == false ) break;
		row++;
	}

	return true;

} // post_change_project_each_row()
