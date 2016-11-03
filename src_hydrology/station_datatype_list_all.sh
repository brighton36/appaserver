#!/bin/sh
# ---------------------------------------------
# station_datatype_list_all.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

station_datatype=`get_table_name $application station_datatype`

echo "	select station, datatype					\
	from $station_datatype						\
	order by station, datatype;"					|
sql '|'									|
cat
