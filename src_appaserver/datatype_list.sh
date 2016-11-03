:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application station" 1>&2
	exit 1
fi

application=$1
station=$2

table_name=`get_table_name $application station_datatype`

echo "select datatype			\
      from $table_name	 		\
      where station = '$station'	\
      order by datatype;" 		|
sql.e
