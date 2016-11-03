:
# hydrology_select_value4validation.sh
# ------------------------------------
if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application station datatype begin_date end_date" 1>&2
	exit 1
fi

application=$1
station=$2
datatype=$3
begin_date=$4
end_date=$5

table_name=`get_table_name $application measurement`

echo "select measurement_date,measurement_time,datatype,measurement_value \
      from $table_name							  \
      where station = '$station'					  \
	and datatype = '$datatype'					  \
	and measurement_date >= '$begin_date'				  \
	and measurement_date <= '$end_date'				  \
      order by measurement_date,measurement_time;"			  |
sql.e ','							  	  |
sed 's/,/=/3'								  |
sed 's/,/|/2'								  |
sed 's/$/=0/'								  |
cat

exit 0
