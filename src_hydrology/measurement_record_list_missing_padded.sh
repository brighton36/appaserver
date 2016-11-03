#!/bin/sh
# src_everglades/measurement_record_list_missing_padded.sh
# --------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 9 ]
then
	echo "Usage: $0 application station begin_date:end_date aggregate_level validation_level aggregate_statistic:accumulate_yn append_low_high_yn units_converted datatype" 1>&2
	exit 1
fi

application=$1
station=$2
begin_colon_end_date=$3
aggregate_level=$4
validation_level=$5
aggregate_statistic_accumulate_yn=$6
append_low_high_yn=$7
units_converted=$8
datatype=$9

begin_date=`echo $begin_colon_end_date | piece.e ':' 0`
end_date=`echo $begin_colon_end_date | piece.e ':' 1`

expected_count_list=`	datatype_expected_count_list.sh		\
		    	$station				\
			$datatype				\
			'^'					|
			joinlines.e '|'`

measurement_record_list.sh	$application				\
				$station				\
				$begin_colon_end_date			\
				$aggregate_level			\
				${validation_level}			\
				${aggregate_statistic_accumulate_yn}	\
				$append_low_high_yn			\
				$units_converted			\
				$datatype				|
piece_shift_left.e '|'							|
piece_shift_left.e '|'							|
pad_missing_times.e	'|'						\
			0,1,2						\
			$aggregate_level				\
			$begin_date					\
			0000						\
			$end_date					\
			2359						\
			2						\
			"$expected_count_list"				|
piece_shift_right.e '|'							|
piece_shift_right.e '|'							|
sed "s/^||/${station}|${datatype}|/"					|
cat

exit 0

