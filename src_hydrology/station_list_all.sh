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

echo "	select station
	from station
	order by station;"					|
`get_appaserver_home.e`/src_appaserver/sql.e $application

exit 0
