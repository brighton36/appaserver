// src_hydrology/post_change_longitude_latitude.js
// -----------------------------------------------

function post_change_longitude_latitude( element )
{
	var degrees;
	var minutes;
	var seconds;
	var seconds_integer;
	var floating_point;
	var string_array;
	var seconds_array;
	var delimiter;
	var minus_sign;
	var element_name_array;
	var local_element_name;

	delimiter = timlib_get_non_minus_delimiter( element.value );

	if ( delimiter == '' )
	{
		alert( 'ERROR: invalid value. Resetting...' );
		element.value = element.defaultValue;
		return true;
	}

	if ( element.name.substr( 0, 9 ) == 'longitude' )
		minus_sign = '-';
	else
		minus_sign = '';

	if ( delimiter == '.' )
	{
		string_array = element.value.split( delimiter );
		if ( string_array.length == 2 )
		{
			degrees = string_array[ 0 ];
			if ( degrees.charAt( 0 ) == '-' )
			{
				degrees = degrees.substr( 1 );
			}
	
			if ( degrees.length == 1 )
			{
				degrees = "0" + degrees;
			}
	
			if ( degrees.length != 2 )
			{
				alert( 'ERROR: invalid value. Resetting...' );
				element.value = element.defaultValue;
				return true;
			}
	
			floating_point = string_array[ 1 ];
			if ( floating_point.length < 0
			||   floating_point.length > 5 )
			{
				alert(
				'ERROR: float must be 5 digits. Resetting...' );
				element.value = element.defaultValue;
				return true;
			}
			else
			if ( floating_point.length == 1 )
				floating_point = floating_point + "0000";
			else
			if ( floating_point.length == 2 )
				floating_point = floating_point + "000";
			else
			if ( floating_point.length == 3 )
				floating_point = floating_point + "00";
			else
			if ( floating_point.length == 4 )
				floating_point = floating_point + "0";
	
			element.value =	minus_sign +
					degrees + '.' +
					floating_point;
			return true;
		}
	}

	string_array = element.value.split( delimiter );
	if ( string_array.length != 3
	&& ( string_array.length == 4 && delimiter != '.' ) )
	{
		alert( 'ERROR: invalid value. Resetting...' );
		element.value = element.defaultValue;
		return true;
	}

	degrees = string_array[ 0 ];
	if ( degrees.charAt( 0 ) == '-' )
	{
		degrees = degrees.substr( 1 );
	}

	if ( degrees.length == 1 )
	{
		degrees = "0" + degrees;
	}

	minutes = string_array[ 1 ];
	if ( minutes.length == 1 )
	{
		minutes = "0" + minutes;
	}

	if ( delimiter != '.' )
	{
		seconds = string_array[ 2 ];
		seconds_array = seconds.split( '.' );
		if ( seconds_array.length != 2 )
		{
			seconds = seconds + '.00';
			seconds_array = seconds.split( '.' );
		}
		seconds_integer = seconds_array[ 0 ];
		if ( seconds_integer.length == 1 )
		{
			seconds_integer = "0" + seconds_integer;
		}
		floating_point = seconds_array[ 1 ];
	}
	else
	{
		seconds_integer = string_array[ 2 ];
		if ( seconds_integer.length == 0 )
		{
			seconds_integer = "00" + seconds_integer;
		}
		else
		if ( seconds_integer.length == 1 )
		{
			seconds_integer = "0" + seconds_integer;
		}
		floating_point = string_array[ 3 ];
	}

	if ( floating_point.length == 0 )
	{
		floating_point = "00" + floating_point;
	}
	else
	if ( floating_point.length == 1 )
	{
		floating_point = "0" + floating_point;
	}
	else
	if ( floating_point.length != 2 )
	{
		alert(
		'ERROR: seconds float must be two digits. Resetting...' );
		element.value = element.defaultValue;
		return true;
	}

	element.value =	minus_sign +
			degrees + ':' +
			minutes + ':' +
			seconds_integer + '.' +
			floating_point;
	return true;
} // post_change_longitude_latitude()

