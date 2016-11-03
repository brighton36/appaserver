#!/bin/sh
# ---------------------------------------------
# src_everglades/fishing_area_entire.sh
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

#application=$1
application=creel

fishing_area=`get_table_name $application fishing_area`

echo "	select $fishing_area.location_area				\
	from $fishing_area						\
	where $fishing_area.location_zone = 'entire'			\
	order by $fishing_area.location_area;"				|
sql.e

exit 0
