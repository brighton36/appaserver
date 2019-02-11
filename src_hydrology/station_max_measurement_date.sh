#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/station_max_measurement_date.sh
# ---------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 station" 1>&2
	exit 1
fi

station=$1

where="station = '$station' and datatype = 'stage'"

results=`echo "select 1 from station_datatype where $where" | sql.e`

if [ "$results" != "" ]
then
	datatype="stage"
else
	datatype="rain"
fi

echo "	select max( measurement_date )				\
	from measurement					\
	where station = '$station' and datatype = '$datatype'"	|
sql.e

exit 0
