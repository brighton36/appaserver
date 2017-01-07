//
// trim.js
// ---------------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function trim_trailing_piece( source_string, delimiter )
{
	var length;

	length = source_string.length;

	if ( !length ) return source_string;

	for ( var i = length - 1; i; i-- )
	{
		if ( source_string.charAt( i ) == delimiter )
		{
			return source_string.substring( 0, i );
		}
	}
	return source_string;
} // trim_trailing_piece()

function left_trim( s )
{
	var return_string = "";
	var i;

	for ( i = 0; i < s.length; i++ )
		if ( s.charCodeAt( i ) != 32 )
			return_string += s.charAt( i );
	return return_string;
} // left_trim()

function right_trim( s )
{
	var i, end;

	if ( s.length == 0 ) return s;

	for ( i = s.length - 1; i; i-- )
	{
		if ( s.charCodeAt( i ) != 32
		&&   s.charCodeAt( i ) != 47 )
		{
			break;
		}
	}
	if ( i == s.length - 1 )
	{
		return s;
	}
	else
	{
		return s.substr( 0, i + 1 );
	}
} // right_trim()

