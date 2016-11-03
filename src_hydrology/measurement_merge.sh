#!/bin/sh
# ---------------------------------------------
# measurement_merge.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application station datatype1 datatype2 \"where\"" 1>&2
	exit 1
fi

application=$1
station=$2
datatype1=$3
datatype2=$4
where_clause=$5

measurement=`get_table_name $application measurement`
temp_file1=/tmp/measurement1_$$.dat
temp_file2=/tmp/measurement2_$$.dat
select="measurement_date,measurement_time,measurement_value"

echo "	select $select
	from $measurement
	where station = '$station'
	  and datatype = '$datatype1'
	  and $where_clause
	order by measurement_date,measurement_time;" 			|
sql.e									|
sed 's/\^/|/2'								|
cat > $temp_file1

echo "	select $select
	from $measurement
	where station = '$station'
	  and datatype = '$datatype2'
	  and $where_clause
	order by measurement_date,measurement_time;" 			|
sql.e									|
sed 's/\^/|/2'								|
cat > $temp_file2

join -t'|' $temp_file1 $temp_file2

rm -f $temp_file1 $temp_file2

exit 0
