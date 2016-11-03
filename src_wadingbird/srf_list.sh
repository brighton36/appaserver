#!/bin/sh
# ---------------------------------------------
# srf_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application role" 1>&2
	exit 1
fi

application=$1
role=$2

systematic_reconnaissance_flight=`get_table_name $application systematic_reconnaissance_flight`

if [ "$role" = "supervisor" ]
then
	where="1 = 1"
else
	where="non_conventional_yn is null or non_conventional_yn <> 'y'"
fi

echo "	select initial_takeoff_date					\
	from $systematic_reconnaissance_flight				\
	where $where							\
	order by initial_takeoff_date;"					|
sql.e									|
cat

exit 0
