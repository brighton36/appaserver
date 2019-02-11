#!/bin/sh
# ---------------------------------------------
# station_datatype_list_all.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

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

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 ignored [station]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	station_where="station = '$2'"
else
	station_where="1 = 1"
fi

station_datatype=`get_table_name $application station_datatype`

echo "	select station, datatype					\
	from $station_datatype						\
	where $station_where						\
	order by station, datatype;"					|
sql '|'									|
cat

