#!/usr/bin/ksh

if [ $# -ne 2 ] ; then

echo ""
echo "Usage $0: station _OR_ run"
echo ""
echo "if you want all stations in a run type "station" for station"
echo "if you want just one station type "run" for run"
echo ""
echo "for example: station gulf_coast _or_ BR run will work"

exit 0 

fi

sql_lib=/opt/dfe/sql
exe_lib=/usr/lib

station=$1
run=$2

#
# if an actual station is entered
#

if [[ $station != station ]]; then

echo "Content-type: text/html"
echo ""
echo "<pre>"

#
#	First get the date of the last visit for
#	the query
#

thedate= $(echo "select max(visit_date) from site_visit	\
	where station='$station' "			\
	group by station				\
|sql ' ')		

#
#	repeat so we get it to stdout
#

echo "select station, max(visit_date)			\
	from site_visit					\
	where station='$station' 			\
	group by station"				\
| sql '	' 						

echo ""

echo "select station, visit_date, visit_time, datatype,	\
	logger_value, check_value, new_value, final_check	\
	from site_visit_calibrate			\
	where visit_date='$thedate'			\
	and station='$station'"			\
| sql '	'

#
#  Else if just a run is entered
#

elif [[ $run != run ]]; then

echo "Content-type: text/html"
echo ""
echo "<pre>"

echo "Last Site Visits for $run stations"
echo ""

for station in $(${sql_lib}/run_stations.sh $run); do

echo "the station is: $station"

thedate=$(echo "select max(visit_date) from site_visit \
	where station='$station' 			\
	group by station "				\
|sql ' ')

echo "the date is: $thedate"

echo "select station, visit_date, visit_time, datatype,	\
	logger_value, check_value, new_value, final_check	\
	from site_visit_calibrate			\
	where visit_date='$thedate'			\
	and station='$station'"			\
| sql '		'

echo ""

done

else 

echo "Content-type: text/html"
echo ""
echo "<pre>"

for station in $(${sql_lib}/enp_active_stations.sh); do
echo "select station, max(visit_date)		\
	from site_visit				\
	where station='$station'		\
	group by station"			\
| sql '		' 			

done

fi

echo "</pre>"


