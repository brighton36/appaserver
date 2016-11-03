:
# src_hydrology/calculate_station_utm_cs2cs.sh
# --------------------------------------------

delimiter=','
default_zone=17

while read input_line
do
	station=$(echo $input_line | piece.e ',' 0)
	longitude=$(echo $input_line | piece.e ',' 1)
	latitude=$(echo $input_line | piece.e ',' 2)
	zone=$(echo $input_line | piece.e ',' 3)

	if [ "$zone" = "" ]
	then
		zone=$default_zone
	fi

	easting_northing=`echo $longitude $latitude | cs2cs_lonlat.sh $zone`
	easting=`echo ${easting_northing} | column.e 0 | piece.e '.' 0`
	northing=`echo ${easting_northing} | column.e 1 | piece.e '.' 0`
	echo	\
	"${station},${longitude},${latitude},${zone},${easting},${northing}"
done

exit 0
