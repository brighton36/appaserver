#!/bin/sh
# ---------------------------------------------
# station_list_bisc.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

station=`get_table_name $application station`

echo "	select station							\
	from $station							\
	where agency = 'NPS_BISC'					\
	order by $station.station;"					|
sql.e									|
toupper.e

exit 0
