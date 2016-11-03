#!/bin/sh
# ---------------------------------
# station_list_all.sh
# ---------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

station=`get_table_name $application station`

echo "	select $station.station					\
	from $station						\
	order by $station.station;"				|
sql.e								|
toupper.e
