#!/usr/bin/ksh
#
# calculate max o2 solubility for all stations where we have
# DO_max datatype assigned and have validated salinity and bottom_temperature data
#
# insert result into database
if [ $# -eq 1 ]; then
	station=$1;
	echo "Set station to $station"; echo "$station" > tmpfile
elif [ $# -gt 1 ]; then
	echo ""; echo "Usage: $0 (station) -- optional, without station will run all stations"
	exit 0
else
	echo "Extracting active station list from database and proceeding with calculation"
	echo ""
	num=$(echo "select count(*) from enp_station,station_datatype where 		\
		enp_station.station=station_datatype.station and 			\
		station_datatype.datatype='DO_Max'" | sql) 
	echo ""; echo "number of stations with datatype DO_Max = $num "
	echo "select enp_station.station from enp_station,station_datatype where 	\
		enp_station.station=station_datatype.station and 			\
		station_datatype.datatype='DO_Max'" | sql > tmpfile
fi
echo "proceeding with data grab"
while read station; do
	echo "working on station $station"
	echo "select station,datatype,measurement_date,measurement_time,measurement_value \
	FROM measurement 					\
	WHERE station='$station' AND datatype='salinity'	\
	AND measurement_date BETWEEN '1990-01-01' AND '2009-12-31'" | sql | sed 's/\^/	/g' > tmpfile2
done < tmpfile
> tmpfile3
while read stn dtype mdate mtime sal; do
	btemp=$(echo "SELECT measurement_value FROM measurement \
		WHERE station='$stn' AND datatype='bottom_temperature'	\
		AND measurement_date='$mdate' AND measurement_time='$mtime'\
		" | sql)
	echo "$stn $mdate $mtime $sal $btemp" >> tmpfile3
done < tmpfile2
cat tmpfile3
echo "starting calculation and inserting results";echo""
date
dtype='DO_Max'
while read stn mdate mtime sal btemp; do
	measurement_value=$(oxygen_solubility.py $sal $btemp)
#	echo "$stn,$dtype,$mdate,$mtime,$measurement_value,$sal,$btemp"
	echo "INSERT INTO measurement (station,datatype,measurement_date,measurement_time,measurement_value) VALUES ('$stn','$dtype','$mdate','$mtime','$measurement_value')" | sql
done < tmpfile3
date
exit 0
