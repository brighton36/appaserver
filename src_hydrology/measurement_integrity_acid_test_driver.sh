#!/bin/bash
# ------------------------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/measurement_integrity_acid_test_driver.sh
# ------------------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 3 ]
then
	echo "Usage: `basename.e $0 n` process measurement_date station" 1>&2
	exit 1
fi

echo "$0 $*" 1>&2

process=$1
measurement_date=$2
station=$3

content_type_cgi.sh

cat << all_done
<html>
<head>
<link rel=stylesheet type="text/css" href="/appaserver/$application/style.css">
</head>
<body>
all_done
 
echo "<html>"
echo "<body>"
echo "<h1>`echo $process | format_initial_capital.e`</h1>"

heading="station,datatype,measurement_date,measurement_time,measurement_value,status,minimum,mean,maximum"

measurement_integrity_acid_test.sh $measurement_date raw $station	|
sed 's/min=//'								|
sed 's/avg=//'								|
sed 's/max=//'								|
html_table.e '' "$heading" '^'						|
cat

echo "</body>"
echo "</html>"

exit 0

