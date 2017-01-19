#!/usr/bin/ksh
#
# Script to get blocks of data the exist.
# gaps between blocks indicate missing data
#
#

if [ $# -ne 5 ]; then

	echo " "
	echo "Usage $0: station datatype start_date end_date count_per_day"
	echo " "

	exit 1
fi

util_dir=/usr/local/util

station=$1
datatype=$2
start_date=$3
end_date=$4
count=$5

#
#	Create HTML wrapper
#

echo "Content-type: text/html"
echo ""

echo "<link rel=stylesheet type="text/css" href=/appaserver/hydrology/style.css> "

echo "<pre>"

if [[ $5 == "count_per_day" ]]; then 

echo "<link rel=stylesheet type="text/css" href=/appaserver/hydrology/style.css> "

echo "Enter a count Please"

echo "</pre>"

exit 1

fi

echo "$station	$datatype"
echo ""

		echo "select measurement_date, measurement_time, measurement_value			\
			from measurement where station='$station'					\
			and datatype='$datatype'							\
			and measurement_value is not null						\
			and measurement_date>= '$start_date'						\
			and measurement_date<= '$end_date'"						|
sql '|' 											|
$util_dir/get_start_end_block_with_count.e $station $datatype 0 1 $count					

echo "</pre>"




	
	
