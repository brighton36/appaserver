#!/bin/bash
# ------------------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/station_measurement_days_missing.sh
# ------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ------------------------------------------------------------------

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

max_measurement_date=`station_max_measurement_date.sh $station`

if [ "$max_measurement_date" = "" ]
then
	echo 4
fi

days_between=$(days_between.sh $max_measurement_date `now.sh ymd`)

results=$(($days_between+1))

echo $results

exit 0
