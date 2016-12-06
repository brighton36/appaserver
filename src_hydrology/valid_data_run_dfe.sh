#!/usr/bin/ksh
#
# Script to pull approximate "last validation" date from 
# each run's station-datatype. Orginally the script was
# *valid_data_blocks_dfe.sh* which was butchered into the
# below by DR (20100817). Now script queries the enp_station
# table to get the stations, uses that info to query the
# station_datatype table for "validation_required_yn"
# datatypes, and outputs the values from those station-
# datatypes for the last 6 months (epoch_date). Lastly,
# a ".e" script is used to get the block_end's which
# is then put through a temporary file (tmp_valid) to 
# get a better output for easy reading. 
# 
#
#

if [ $# -ne 1 ]; then

	echo " "
	echo "Usage $0: run"
	echo " "

	exit 1
fi

script_dir=/opt/dfe/sql
util_dir=/usr/local/util

run=$1
epoch_date=$( echo "`date +%s` - 15778458" | bc )
start_date=$( date -d @$epoch_date +%Y-%m-%d )
end_date=$( date +%Y-%m-%d )
run_station=$( echo "SELECT station FROM enp_station WHERE run='$run'" | sql )
block_executable=get_start_end_block.e
count=""
> tmp_valid

# [epoch_date] is how it calculates back to the start_date - 15778458 is the number
# of seconds in 6 months.
# [run_station] is used to provide a station list to primary_datatypes.sh per run
# [block_executable] is used to find beginning/ending dates from a data query
# [count] is needed by the get_start_end_block.e script but it has been "removed"
# [tmp_valid] is the temporary file created to handle a better output. This is
# probably a bad "hack because if two people happen to run this process at the same
# time - I'm not sure what will happen.......DR 20100817
#
#
#	Create HTML wrapper
#

echo "Content-type: text/html"
echo ""

echo "<pre>"

for station in $run_station; do

	for datatype in $($script_dir/primary_datatypes.sh $station); do

#	echo "$station	$datatype"

#	echo ""
	end_validation=$( echo "select max(measurement_date) 							\
				from measurement where station='$station'					\
				and datatype='$datatype'							\
				and measurement_date>= '$start_date'						\
				and last_validation_date is not null"						|
	sql )

	print $station $datatype $end_validation 

	done

print

done
