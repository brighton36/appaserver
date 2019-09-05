#!/bin/bash
# src_hydrology/measurement_integrity_acid_test.sh
# ------------------------------------------------
# Freely available software: see Appaserver.org
# ------------------------------------------------

measurement_acid_test_standard_deviation=3.2
days_before=90

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

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 measurement_date output_medium [station]" 1>&2
	echo "Output_medium = {raw,padded}" 1>&2
	exit 1
fi

measurement_date=$1
output_medium=$2

if [ "$output_medium" != "raw" -a "$output_medium" != "padded" ]
then
	echo "Output_medium = {raw,padded}" 1>&2
	exit 1
fi

if [ "$#" -eq 3 -a "$3" != "station" -a "$3" != "" ]
then
	station_grep="grep ^$3\^"
else
	station_grep="cat"
fi

if [ "$output_medium" = "raw" ]
then
	output_process="cat"
elif [ "$output_medium" = "padded" ]
then
	output_process="measurement_integrity_acid_test_format.sh"
else
	output_process="cat"
fi

begin_date=`date_subtract_days.sh $measurement_date $days_before`
end_date=`date_subtract_days.sh $measurement_date 1`
today_file="/tmp/measurement_today_file_$$.dat"

results=`application_constant.sh measurement_acid_test_standard_deviation`

if [ "$results" != "" ]
then
	measurement_acid_test_standard_deviation=$results
fi

(
# Execute with flow datatypes (aggregate_sum_yn = 'n')
# ----------------------------------------------------
station_datatype_aggregate_list.sh n					|
$station_grep								|
while read record
do
	station=`echo $record | piece.e '^' 0`
	datatype=`echo $record | piece.e '^' 1`

	measurement_real_time_list.sh	$station			\
					$datatype			\
					$measurement_date		\
					$measurement_date		|
	sed 's/\^/|/1'							|
	sed 's/\^/|/1'							|
	sed 's/\^/|/1'							|
	cat > $today_file

	measurement_real_time_list.sh	$station			\
					$datatype			\
					$begin_date			\
					$end_date			|
	piece.e '^' 4							|
	exceed_standard_deviation.e					\
			$measurement_acid_test_standard_deviation	\
			key_string value				\
			'^'						\
			$today_file					|
	tr '|' '^'							|
	$output_process							|
	cat
done

# Execute with quantum datatypes (aggregate_sum_yn = 'y')
# ------------------------------------------------------------
station_datatype_aggregate_list.sh y					|
$station_grep								|
while read record
do
	station=`echo $record | piece.e '^' 0`
	datatype=`echo $record | piece.e '^' 1`

	measurement_daily_sum_list.sh	$station			\
					$datatype			\
					$measurement_date		\
					$measurement_date		|
	sed 's/\^/|/1'							|
	sed 's/\^/|/1'							|
	cat > $today_file

	measurement_daily_sum_list.sh	$station			\
					$datatype			\
					$begin_date			\
					$end_date			|
	piece.e '^' 3							|
	exceed_standard_deviation.e					\
			$measurement_acid_test_standard_deviation	\
			key_string value				\
			'^'						\
			$today_file					|
	tr '|' '^'							|
	$output_process							|
	cat
done
) | sort

rm $today_file

exit 0

