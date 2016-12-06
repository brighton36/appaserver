#!/usr/bin/ksh
#
# This script will provide a list of the last
# rain gage activity for each station in a run
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
start_date=$( $util_dir/one_year_ago.sh )

run_station=$( echo "SELECT station FROM enp_station WHERE run='$run'" | sql )

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


#	echo "$station	$datatype"

#	echo ""
	last_rain_date=$( echo "select max(date)				\
				from rain_gage_cal where station='$station'"	|	
	sql )

	last_rain_act=$( echo "select rain_gage_acitivity			\
				from rain_gage_cal where station='$station'	\
				and date= '$last_rain_date'"			|	
	sql )
	print $station $last_rain_date $last_rain_act 

print

done
