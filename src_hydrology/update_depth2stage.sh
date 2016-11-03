:
# update_depth2stage.sh
# ---------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 station"
	exit 1
fi

station=$1

echo "	select station,datatype,measurement_date,measurement_time	\
	from measurement						\
	where station = '$station'					\
	  and datatype = 'depth';" 					|
mysql.sh ',' 								|
sed 's/$/|datatype=stage/' 						|
update.e measurement station,datatype,measurement_date,measurement_time	|
mysql.sh

