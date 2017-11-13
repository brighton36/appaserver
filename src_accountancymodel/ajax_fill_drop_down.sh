#!/bin/bash
# ------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/ajax_fill_drop_down.sh
# ------------------------------------------------------------

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 application login_name role session one2m_folder mto1_folder select" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

login_name=$2
role=$3
session=$4
one2m_folder=$5
mto1_folder=$6
select=$7

function output_html_header()
{
	application=$1

cat << all_done

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >

<head>

<link rel=stylesheet type="text/css" href="/appaserver/$application/style.css">

all_done

}
# function output_html_header()

function output_script_header()
{
	application=$1
	login_name=$2
	role=$3
	one2m_folder=$4
	mto1_folder=$5

cat << all_done

<script language="javascript" type="text/javascript">

function fork_ajax_window( destination_element_id )
{
	var my_window;
	var my_document;
	var HTMLstring;

HTMLstring = '<HTML>\n';
HTMLstring += '<HEAD>\n';
HTMLstring += '<link rel=stylesheet type="text/css" href="/appaserver/$application/style.css">'
HTMLstring += '<TITLE>${one2m_folder}</TITLE>\n';

HTMLstring += '<script language="javascript" type="text/javascript">\n';
HTMLstring += 'var http_request = new XMLHttpRequest();\n';
HTMLstring += 'function listen_function()\n';
HTMLstring += '{\n';
HTMLstring += '	var ready;\n';
HTMLstring += '	var status;\n';
HTMLstring += '	var results;\n';
HTMLstring += '	var e;\n';
HTMLstring += '	var a;\n';
HTMLstring += '	ready = http_request.readyState;\n';
HTMLstring += '	status = http_request.status;\n';
HTMLstring += '	if ( status != 200 ) return;\n';
HTMLstring += '	if ( ready != 4 ) return;\n';
HTMLstring += '	results = http_request.responseText;\n';
HTMLstring += '	a = results.split( "^" );\n';

HTMLstring += '	e = window.opener.document.getElementById( \"';
HTMLstring += destination_element_id;
HTMLstring += '\" );\n';

HTMLstring += '	for ( var i = 0; i < a.length; i++ )\n';
HTMLstring += '		e.options[ i ] = new Option( a[ i ], a[ i ] );\n';

HTMLstring += 'window.close()\n';
HTMLstring += '	return true;\n';
HTMLstring += '}\n';

HTMLstring += 'function send_to_server()\n';
HTMLstring += '{\n';
HTMLstring += '	var action;\n';
HTMLstring += '	var element;\n';
HTMLstring += '	var value;\n';
HTMLstring += '	element = document.getElementById( "${one2m_folder}" );\n';
HTMLstring += '	value = element.value;\n';
HTMLstring += '	action = "/cgi-bin/post_ajax_fill_drop_down?${application}+${login_name}+${role}+${session}+${mto1_folder}+${one2m_folder}+" + value;\n';
HTMLstring += '	http_request.onreadystatechange = listen_function;\n';
HTMLstring += '	http_request.open( "GET", action, true );\n';
HTMLstring += '	http_request.send(null);\n';
HTMLstring += '	return true;\n';
HTMLstring += '}\n';

// Need to escape the 's' because script is a reserved word.
// ---------------------------------------------------------
HTMLstring += '</\script>\n';

HTMLstring += '</HEAD>\n';
HTMLstring += '<BODY>\n';

HTMLstring += '<form>\n';
HTMLstring += '${one2m_folder}\n';
HTMLstring += '<select name="${one2m_folder}" id="${one2m_folder}">\n';

all_done

}
# end function output_script_header()

function output_one2m_folder()
{
	application=$1
	one2m_folder=$2
	select=$3

echo "HTMLstring += '<option value=\"select\">Select\n';"

	echo "select $select from ${one2m_folder} order by $select;" | sql.e |
	while read results
do
echo "HTMLstring += '<option value=\"${results}\">${results}\n';"
done

echo "HTMLstring += '</select>\n';"

}
# end function output_one2m_folder()

function output_script_footer()
{
	one2m_folder=$1
	mto1_folder=$2

cat << all_done
HTMLstring += '<input type=button value="Submit" onClick="send_to_server()" />\n';
HTMLstring += '</form>\n';

HTMLstring += '</BODY>\n';
HTMLstring += '</HTML>';

	my_window = window.open( "", "${one2m_folder}", "width=400,height=300" );
	my_document = my_window.document;
	my_document.write( HTMLstring );
	my_document.close();

	return true;
}

</script>

all_done

}
# function output_script_footer()

function output_body()
{
cat << all_done

</head>

<body>

Choose a ${mto1_folder}:
<select name="${mto1_folder}" id="${mto1_folder}" /> </select>
<input type=button value="Fill" onClick="fork_ajax_window( '${mto1_folder}' )" />

</body>
</html>

all_done

}
# end function output_body()

output_html_header $application
output_script_header	$application		\
			"$login_name"		\
			"$role"			\
			$one2m_folder		\
			$mto1_folder

output_one2m_folder $application $one2m_folder "$select"
output_script_footer $one2m_folder $mto1_folder
output_body $one2m_folder $mto1_folder

exit 0
