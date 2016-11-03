:
# src_hydrology/agency_station_list.sh
# ------------------------------------
if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application [agency]" 1>&2
	exit 1
fi

application=$1

if [ "$#" -eq 2 -a "$2" != "" ]
then
	where="agency='$2'"
else
	where="1=1"
fi

agency_table=`get_table_name $application agency`
station=`get_table_name $application station`
agency_order="order by agency"
station_order="order by station"

for agency in `
	echo "select agency from $agency_table where $where $agency_order;" |
	sql.e								    |
	cat`
do
	agency_where="where agency = '$agency'"
	/bin/echo -n "$agency|"
	echo "select station from $station $agency_where $station_order;"   |
	sql.e								    |
	joinlines.e ','
	/bin/echo

done									    |
grep -v '^$'								    |
cat

exit 0
