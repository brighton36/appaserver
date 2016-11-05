#!/bin/sh
# ---------------------------------------------
# output_blank_screen.sh
# ---------------------------------------------
#
# This is used as a filler for frames.
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# echo "starting $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 bgcolor title output_content_type_yn" 1>&2
	exit 1
fi

bgcolor=$1

if [ "$2" != "" ]
then
	title="<h1>$2</h1>"
else
	title=""
fi

if [ "$3" = "y" ]
then
	content_type_cgi.sh
fi

cat << all_done
<HTML>
<HEAD>
        <link rel=stylesheet type="text/css" href=style.css>
</HEAD>

<body bgcolor=$bgcolor>
$title
</body>
</HTML>
all_done

exit 0
