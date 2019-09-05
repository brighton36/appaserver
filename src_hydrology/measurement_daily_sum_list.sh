#!/bin/sh
# src_hydrology/measurement_daily_sum_list.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 station datatype begin_date end_date" 1>&2
	exit 1
fi

station=$1
datatype=$2
begin_date=$3
end_date=$4

measurement=`get_table_name $application measurement`

select="station,datatype,measurement_date,measurement_time,measurement_value"

echo "	select	$select							\
      	from $measurement 						\
      	where station = '$station'					\
       	  and datatype = '$datatype'					\
	  and measurement_date >= '$begin_date'				\
	  and measurement_date <= '$end_date'				\
	  and ifnull(measurement_value,0) > 0				\
	order by $select;"						|
	sql_quick.e '^'							|
	real_time2aggregate_value.e					\
		sum 2 3 4 '^' daily n $end_date 2>/dev/null		|
	piece_inverse.e 5 '^'						|
	cat

exit 0
