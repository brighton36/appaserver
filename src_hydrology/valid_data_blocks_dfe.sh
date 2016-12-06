#!/usr/bin/ksh
#
# Script to get blocks of Valid data the exist.
# gaps between blocks indicate un-validated data
#

if [ $# -ne 3 ]; then

	echo " "
	echo "Usage $0: station start_date end_date"
	echo " "

	exit 1
fi

script_dir=/opt/dfe/sql
##util_dir=/opt/physical/linux/utility
util_dir=/usr/local/util

station=$1
start_date=$2
end_date=$3

# count=$4
# Removed count here and replaced it in the if statement so that
# count_per_day can be hourly for data by default and user selected for stage
#	Create HTML wrapper
#

echo "Content-type: text/html"
echo ""

echo "<pre>"

for datatype in $($script_dir/primary_datatypes.sh $station); do

##if [[ $datatype = stage ]]; then
 ##       block_executable=get_start_end_block_with_count.e
  ##      count=$4
##else
        block_executable=get_start_end_block.e
        count=""
##fi

echo "$station	$datatype "

		echo "select measurement_date, measurement_time, measurement_value			\
			from measurement where station='$station'					\
			and datatype='$datatype'							\
			and measurement_date>= '$start_date'						\
			and measurement_date<= '$end_date'						\
			and last_validation_date is not null"						|
sql '|' 												|
##sed 's/|/ | /g' 											|
$util_dir/$block_executable $station $datatype 0 1 

echo ""

done

echo "</pre>"


	
