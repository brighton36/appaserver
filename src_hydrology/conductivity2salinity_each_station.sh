#!/bin/bash
# src_hydrology/conductivity2salinity_each_station.sh
# --------------------------------------------------------
# This script calculates and inserts salinity measurements
# for each station.
# It starts at a station's last salinity record
# date and ends at that station's last
# conductivity date.
#
# All of the conductivity name variants are traversed.
# For example, "conductivity_bottom".
# --------------------------------------------------------

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 application login_name [station]" 1>&2
	exit 1
fi

echo "$0 $*" 1>&2

application=$1
login_name=$2

if [ "$#" -eq 3 ]
then
	station_parameter=$3
else
	station_parameter=""
fi

measurement_table=`get_table_name $application measurement`
station_datatype_table=`get_table_name $application station_datatype`
conductivity_salinity_map_table=`get_table_name			\
				$application			\
				conductivity_salinity_map`

function conductivity2salinity_datatype()
{

	conductivity=$1
	salinity=$2
	station_parameter=$3

	echo "$conductivity --> $salinity" 1>&2

	for station in `echo "	select station				\
				from $station_datatype_table		\
				where datatype = '$salinity';"		|
			sql.e`
	do
		if [ "${station_parameter}" != "" ]
		then
			if [ "${station_parameter}" != "$station" ]
			then
				continue
			fi
		fi

		/bin/echo -n "Station = $station: " 1>&2
	
		begin_date=`	echo "	select max( measurement_date )	\
					from $measurement_table		\
					where station = '$station'	\
					  and datatype = '$salinity';"	|
				sql.e`

		if [ "$begin_date" = "" ]
		then
			begin_date="1913-12-31"
		fi

		end_date=`	echo "	select max( measurement_date )	   \
					from $measurement_table		   \
					where station = '$station'	   \
					  and datatype = '$conductivity';" |
				sql.e`
	
#		begin_date=`	echo $begin_date		       |
#				date_add_days.e 1 0 0 '^'	       |
#				piece.e '^' 0`
	
#		if [ `strcmp.e "$begin_date" "$end_date"` -ge 0 ]
#		then
#			echo "All caught up." 1>&2
#			continue
#		fi

		echo "From: $begin_date To: $end_date" 1>&2

		conductivity2salinity					\
		$login_name						\
		$application 						\
		$station 						\
		"conductivity_datatype_1=$conductivity&			\
		 from_measurement_date='$begin_date'&			\
		 relation_operator_measurement_date=between&		\
		 to_measurement_date='$end_date'"			\
		"station = '$station' and				\
		 measurement_value is not null and			\
		 measurement_date between				\
			'$begin_date' and '$end_date'"			\
		y							\
		nohtml
	
		if [ "$?" -ne 0 ]
		then
			echo "$0 exiting early" 1>&2
			exit 1
		fi
	done
}

select="select conductivity_datatype,salinity_datatype"
from="from $conductivity_salinity_map_table"

for record in `echo "$select $from;" | sql.e`
do
	conductivity=`echo $record | piece.e '^' 0`
	salinity=`echo $record | piece.e '^' 1`
	conductivity2salinity_datatype	"$conductivity"		\
					"$salinity"		\
					"$station_parameter"
done

exit 0
