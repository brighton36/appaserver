:

OXYGEN_SOLUBILITY_DATATYPE="oxygen_solubility"
errorfile="/tmp/$0.err"

function maximum_bottom_temperature_function()
{
	station=$1

	maximum_bottom_temperature=`echo "select max(measurement_date)
				from measurement
				where station = '$station'
				  and datatype = 'bottom_temperature';" |
			  sql.e`
}

function minimum_bottom_temperature_function()
{
	station=$1

	minimum_bottom_temperature=`echo "select min(measurement_date)
				from measurement
				where station = '$station'
				  and datatype = 'bottom_temperature';" |
			  sql.e`
}

function maximum_salinity_function()
{
	station=$1

	maximum_salinity=`echo "	select max(measurement_date)
					from measurement
					where station = '$station'
					  and datatype = 'salinity';" |
			  sql.e`
}

function minimum_salinity_function()
{
	station=$1

	minimum_salinity=`echo "	select min(measurement_date)
					from measurement
					where station = '$station'
					  and datatype = 'salinity';" |
			  sql.e`
}

function oxygen_solubility_station_datatype ()
{
	station=$1

	echo "	insert into station_datatype
		(station,datatype)
		values
		('$station','$OXYGEN_SOLUBILITY_DATATYPE' );"	|
	sql.e 2>&1 | grep -vi duplicate

}

function oxygen_solubility_execute ()
{
	station=$1
	minimum_date=$2
	maximum_date=$3

	where="station = '$station' and measurement_date between '$minimum_date' and '$maximum_date'"

	delete_where="$where and datatype = '$OXYGEN_SOLUBILITY_DATATYPE'"

	echo "delete from measurement where $delete_where;"		|
	sql.e

	calculate_oxygen_solubility	"hydrology"			\
					$station			\
					"$where"			\
					sql				\
					2>>$errorfile			|
	sql.e

}

function oxygen_solubility_maximum ()
{
	station=$1

	maximum_salinity_function $station
	maximum_bottom_temperature_function $station

	results=`strcmp.e $minimum_salinity $minimum_bottom_temperature`
	if [ $results -gt 0 ]
	then
		minimum_date=$minimum_salinity
	else
		minimum_date=$minimum_bottom_temperature
	fi

	results=`strcmp.e $maximum_salinity $maximum_bottom_temperature`
	if [ $results -lt 0 ]
	then
		maximum_date=$maximum_salinity
	else
		maximum_date=$maximum_bottom_temperature
	fi

	oxygen_solubility_execute $station $minimum_date $maximum_date
	oxygen_solubility_station_datatype $station
}

function oxygen_solubility_minimum ()
{
	station=$1

	minimum_bottom_temperature_function $station

	if [ "$minimum_bottom_temperature" != "" ]
	then
		oxygen_solubility_maximum $station
	fi
}

function oxygen_solubility_station ()
{
	station=$1
	minimum_salinity_function $station

	if [ "$minimum_salinity" != "" ]
	then
		oxygen_solubility_minimum $station
	fi
}

function oxygen_solubility_agency ()
{
	agency=$1

	for station in `agency_station_list.sh hydrology $agency	|
			piece.e '|' 1					|
			tr ',' ' '`
	do
		echo "Checking station: $station" >>$errorfile
		oxygen_solubility_station $station
	done
}

echo "	select count(*)
	from measurement
	where datatype = '$OXYGEN_SOLUBILITY_DATATYPE';"	|
	sql.e >$errorfile

for agency in `agency_station_list.sh hydrology | piece.e '|' 0`
do
	echo "Agency: $agency" >>$errorfile
	oxygen_solubility_agency $agency
done

#oxygen_solubility_station BISC36
#oxygen_solubility_station WW
#oxygen_solubility_agency NPS_ENP

echo "	select count(*)
	from measurement
	where datatype = '$OXYGEN_SOLUBILITY_DATATYPE';"	|
	sql.e >>$errorfile

exit 0
