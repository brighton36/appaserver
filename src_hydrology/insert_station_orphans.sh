:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1
station=`get_table_name $entity station`
station_datatype=`get_table_name $entity station_datatype`
shef_datatype=`get_table_name $entity shef_datatype`

for station_name in `echo "select distinct station from $station_datatype;" |\
		     sql`
do
	count_in_station=`echo "select count(*)				     \
			        from $station 				     \
				where station = '$station_name';" 	    |\
			  sql`
	if [ "$count_in_station" -eq 0 ]
	then
		echo "station_datatype orphan: $station_name"
		echo "$station_name" | insert.e $station station | sql
	fi
done

for station_name in `echo "select distinct station from $shef_datatype;"    |\
		     sql`
do
	count_in_station=`echo "select count(*)				     \
			        from $station 				     \
				where station = '$station_name';" 	    |\
			  sql`
	if [ "$count_in_station" -eq 0 ]
	then
		echo "shef_datatype orphan: $station_name"
		echo "$station_name" | insert.e $station station | sql
	fi
done
