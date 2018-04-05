#!/bin/sh
# ---------------------------------
# station_utm_list.sh
# ---------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

station_file="/tmp/station.dat"
utm_file="/tmp/utm.dat"
lonlat_file="/tmp/lonlat.dat"

where="	long_nad83 is not null and
	lat_nad83 is not null and
	long_nad83 like '-%' and
	long_nad83 not like '-999%' and
	long_nad83 not like '-%-%'"

order="station"

echo "select station from station where $where order by $order;"	|
sql.e $application							|
cat > $station_file

select="long_nad83,lat_nad83"

echo "select $select from station where $where order by $order;"	|
sql.e $application							|
cat > $lonlat_file

cat $lonlat_file							|
tr '^' ' '								|
cs2cs_lonlat.sh 2>&1							|
tr ' ' '^'								|
cat > $utm_file

integrity=`	wc -l $station_file $lonlat_file $utm_file		|
		grep -v total						|
		column.e 0						|
		sort -u							|
		wc -l`

if [ $integrity -ne 1 ]
then
	echo "Error: there's a bad $select in station" 1>&2
	exit 1
fi

joinfiles.e $station_file $utm_file '^'

rm $station_file
rm $utm_file
rm $lonlat_file

exit 0
