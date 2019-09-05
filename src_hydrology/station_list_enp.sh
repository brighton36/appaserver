#!/bin/sh
# ---------------------------------------------
# station_list_enp.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

#echo "Starting: $0 $*" 1>&2

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

station=`get_table_name $application station`

echo "	select station							\
	from $station							\
	where agency = 'NPS_ENP'					\
	order by $station.station;"					|
sql.e									|
toupper.e

exit 0
