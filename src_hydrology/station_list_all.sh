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

DATABASE=$application

echo "	select station
	from station
	order by station;"					|
sql.e $DATABASE							|
toupper.e

exit 0
