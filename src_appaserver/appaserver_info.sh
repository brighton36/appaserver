:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application login_name" 1>&2
	exit 1
fi

application=`echo $1 | piece.e ':' 0`
login_name=$2

horizontal_menu_yn=`horizontal_menu_yn.e $application $login_name`

if [ "$horizontal_menu_yn" != 'y' ]
then
cat <<all_done1
Content-type: text/html

all_done1
fi

cat <<all_done2
<html><head>

<!-- Freely available software: see Appaserver.org -->

<link rel=stylesheet type="text/css" href=/$application/style.css>
<TITLE>Appaserver Info</TITLE>
</head>
<body bgcolor="white" onload="window.open('http://appaserver.org','appaserver_info','menubar=no,resizeable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');">
<h1>Appaserver Info</h1>
<p><a href=http://appaserver.org target=_new>Click here</a> to view the Appaserver home page.
</body>
</html>
all_done2

exit 0
