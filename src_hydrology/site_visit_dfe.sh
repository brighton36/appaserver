#!/usr/bin/ksh
#
# Script to generate Site Visit Report for each Tech
#
#

echo "$0 $*" 1>&2

if [ $# -ne 3 ];then

	echo " "
	echo "Usage $0: station start_date end_date"
	echo " "

	exit 1
fi

station=$1
start_date=$2
end_date=$3

echo "Content-type: text/html"
echo ""
echo "<pre>"

echo "$station"

printf "%-10s%-15s%-10s\t\t%-8s%8s%8s%8s%8s\n" Station Datatype Date:Time Logger Check Diff New Check

echo "select station, datatype, 		\
	visit_date, visit_time, logger_value,	\
	check_value, (logger_value - check_value), new_value, final_check		\
	from site_visit_calibrate				\
	where station='$station'			\
	and visit_date between '$start_date' and '$end_date'		\
	order by datatype, visit_date"			|
sql							|
sed 's/temperature/temp/g'				|
sed 's/rain_cumulative/rain_c/g'				|
awk 'BEGIN {FS = "^"} {printf("%-10s%-15s%-10s:%4s\t%8.2f%8.2f%8.2f%8s%8s\n", $1,$2,$3,$4,$5,$6,$7,$8,$9)}' 

echo "Notes"

echo "select station, visit_date, notepad from site_visit where station='$station'	\
	and visit_date between  '$start_date' and '$end_date'"   	|
sql '	'								|
sed 's/</\&lt;/g'							|
cat
		
echo "</pre>"
