#!/usr/bin/ksh
#
# This script will get the expected count per
#	day from the station_datatype table
#
if [ $# -ne 2 ]; then

echo ""
echo "Usage: $0 station datatype"
echo ""

fi

echo "select expected_count from station_datatype	\
	where station='$1'				\
	and datatype='$2'"				|
sql


