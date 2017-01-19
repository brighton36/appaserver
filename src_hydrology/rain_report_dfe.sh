#!/usr/bin/ksh
#
#	Report of one days rainfall
#
#

umask 000

sql_dir=/opt/physical/sql
wd=/tmp

if [ $# -ne 1 ]; then 

echo "Usage $0: date "
exit 0

fi

theday=$1

output_file=$wd/rain_report_$theday

# Clear the output file 
#---------------------#

> $output_file

# Save the output filename to use in the mail script #
#----------------------------------------------------#

echo $output_file > $wd/rain_report_filename

echo "Generating the RainReport for the Marsh"  >/dev/null

for station in $( ${sql_dir}/enp_marsh_rain_stations.sh ); do

echo "select measurement.station, measurement_date, enp_station.run, sum(measurement_value) theValue	\
	from measurement, enp_station			\
	where enp_station.station=measurement.station	\
	and measurement.station='$station'		\
	and datatype='rain'				\
	and measurement_date='$theday'			\
	group by measurement.station, measurement_date	\
	order by theValue, run, station"		|
sql							|
sed 's/\^/	/g'					|
awk '{printf("%-10s\t%10s\t%-20s\t%5s\n", $1, $2, $3, $4)}'	\
>> $output_file

done

echo "" > $wd/tmp1
echo "<H1 align=left>" >> $wd/tmp1
echo "		EverGlades National Park Rain Report" >> $wd/tmp1
echo "		------------------------------------" >> $wd/tmp1
echo "</H1>" >> $wd/tmp1
echo "" >> $wd/tmp1
echo "<h2 align="justify">" >>$wd/tmp1
echo "Marsh Stations:" >>$wd/tmp1
echo "</h2>" >>$wd/tmp1

cat $output_file |sort -r -k 3,4 >>$wd/tmp1

echo "" >> $wd/tmp1

# Repeat for the Marine Stations #
#--------------------------------#

echo "Generating the RainReport for the Marine " >>2

> $wd/tmp2

for station in $( $sql_dir/enp_marine_rain_stations.sh ); do

echo "select measurement.station, measurement_date, enp_station.run, sum(measurement_value) theValue	\
	from measurement,enp_station			\
	where enp_station.station=measurement.station	\
	and measurement.station='$station'		\
	and datatype='rain'			\
	and measurement_date='$theday'	\
	group by measurement.station, measurement_date	\
	order by theValue, run, station"			|
sql						|
sed 's/\^/	/g'				|
awk '{printf("%-10s\t%10s\t%-20s\t%5s\n", $1, $2, $3, $4)}'	\
>> $wd/tmp2

done

echo "<h2>" >$wd/tmp3
echo "Marine Stations:" >> $wd/tmp3
echo "</h2>" >>$wd/tmp3

cat $wd/tmp2 |sort -r -k 3,4 >>$wd/tmp3

cat $wd/tmp1 $wd/tmp3 > $output_file

##rm $wd/tmp*

echo "Content-type: text/html"
echo ""
echo "<link rel=stylesheet type="text/css" href=/appaserver/hydrology/style.css> "
echo "<pre>"

cat $output_file

echo "</pre>"
