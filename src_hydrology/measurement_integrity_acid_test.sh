#!/bin/sh
# src_hydrology/measurement_integrity_acid_test.sh
# ------------------------------------------------
# Freely available software: see Appaserver.org
# ------------------------------------------------

standard_deviation_count=3
days_before=180

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

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 measurement_date [station]" 1>&2
	exit 1
fi

measurement_date=$1

if [ "$#" -eq 2 ]
then
	station_grep="grep ^$2\^"
else
	station_grep="cat"
fi

begin_date=`date_subtract_days.sh $measurement_date $days_before`
end_date=`date_subtract_days.sh $measurement_date 1`
today_file="/tmp/measurement_today_file.dat"

station_datatype_aggregate_average_list.sh	|
$station_grep					|
while read record
do
	station=`echo $record | piece.e '^' 0`
	datatype=`echo $record | piece.e '^' 1`

	measurement_real_time_list.sh	$station		\
					$datatype		\
					$measurement_date	\
					$measurement_date	|
	sed 's/\^/|/1'						|
	sed 's/\^/|/1'						|
	sed 's/\^/|/1'						|
	cat > $today_file

	measurement_real_time_list.sh	$station			\
					$datatype			\
					$begin_date			\
					$end_date			|
	piece.e '^' 4							|
	exceed_standard_deviation.e	$standard_deviation_count	\
					key_string value		\
					'^'				\
					$today_file			|
	tr '|' '^'							|
	cat
done

exit 0

