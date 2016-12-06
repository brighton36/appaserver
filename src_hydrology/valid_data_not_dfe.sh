#!/usr/bin/ksh
#
# Script to get blocks of Valid data the exist.
# gaps between blocks indicate un-validated data
#
#

if [ $# -ne 4 ]; then

	echo " "
	echo "Usage $0: station datatype start_date end_date"
	echo " "

	exit 1
fi

util_dir=/usr/local/util

station=$1
datatype=$2
start_date=$3
end_date=$4

#
#	Create HTML wrapper
#

echo "Content-type: text/html"
echo ""

echo "<pre>"

echo "$station	$datatype"
echo ""
		echo "select station, datatype, measurement_date, 					\
			measurement_time, measurement_value						\
			from measurement where station='$station'					\
			and datatype='$datatype'							\
			and measurement_date>= '$start_date'						\
			and measurement_date<= '$end_date'						\
			and last_validation_date is null"						|
sql '|' 												|
sed 's/|/ | /g' 											

echo ""

echo "</pre>"


	
