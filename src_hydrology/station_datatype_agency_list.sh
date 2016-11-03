#!/bin/sh
# ------------------------------------------------------------
# station_datatype_agency_list.sh
# ------------------------------------------------------------
# This appends the datatype's units.
# Freely available software: see Appaserver.org
# ------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 3 ]
then
	echo "Usage: $0 application login_name process [station] [datatype] [plot_for_station_check_yn] [dont_filter_manipulate_yn] [validation_required_yn]" 1>&2
	exit 1
fi

application=$1
login_name=$2
process_name=$3

station_table=`get_table_name $application station`
station_datatype=`get_table_name $application station_datatype`
datatype=`get_table_name $application datatype`
process=`get_table_name $application process`
appaserver_user_agency=`get_table_name $application appaserver_user_agency`

if [ "$#" -ge 4 -a "$4" != "station" ]
then
	station_and_clause="and ($station_datatype.station in (`single_quotes_around.e $4`)"
else
	station_and_clause="and (1 = 1"
#	if [ "$#" -ge 5 -a "$5" != "datatype" ]
#	then
#		station_and_clause="and (1 = 2"
#	else
#		station_and_clause="and (1 = 1"
#	fi
fi

if [ "$#" -ge 5 -a "$5" != "datatype" ]
then
	datatype_and_clause="and $station_datatype.datatype in (`single_quotes_around.e $5`))"
else
	datatype_and_clause="and 1 = 1)"
fi

if [ "$#" -ge 6 -a "$6" != "plot_for_station_check_yn" ]
then
	plot_for_station_check_yn_and_clause="and $station_datatype.plot_for_station_check_yn = '$6'"
else
	plot_for_station_check_yn_and_clause="and 1 = 1"
fi

if [ "$#" -ge 7 -a "$7" = "y" ]
then
	dont_filter_manipulate_yn="y"
fi

if [ "$#" -ge 8 -a "$8" != "validation_required_yn" ]
then
	validation_required_yn_and_clause="and $station_datatype.validation_required_yn = '$8'"
else
	validation_required_yn_and_clause="and 1 = 1"
fi

process_group=`echo "	select process_group
			from $process
			where process = '$process_name';" |
	       sql.e`

if [ "$dont_filter_manipulate_yn" != "y" -a "$process_group" = "manipulate" ]
then
	agency=`echo "	select agency
			from $appaserver_user_agency
			where login_name = '$login_name'
			  and filter_manipulate_yn = 'y'" |
	        sql.e					  |
	        tr '\n' ','				  |
		sed 's/,$//'				  |
	        single_quotes_around.e`

	if [ "$agency" != "" ]
	then
		station_datatype_and_clause="and manipulate_agency in ($agency)"
	else
		station_datatype_and_clause="and 1 = 1"
	fi
else
	station_datatype_and_clause="and 1 = 1"
fi

select="	$station_table.station,					\
		$station_datatype.datatype,				\
		concat(	' [',						\
			$datatype.units,				\
			'--',						\
			$station_table.agency,				\
			']' )"

echo "	select $select							\
	from $datatype, $station_table, $station_datatype,		\
		$appaserver_user_agency					\
	where $station_table.agency = $appaserver_user_agency.agency	\
	  and $station_table.station = $station_datatype.station	\
	  and $appaserver_user_agency.login_name = '$login_name'	\
	  and $station_datatype.datatype = $datatype.datatype		\
	  $station_and_clause						\
	  $datatype_and_clause						\
	  $plot_for_station_check_yn_and_clause				\
	  $validation_required_yn_and_clause				\
	  $station_datatype_and_clause					\
	order by $station_table.station, $station_datatype.datatype;"	|
sql.e '^'								|
sed 's/\^//2'								|
sed 's/\^//2'								|
sed 's/\^//2'								|
sed 's/\^//2'								|
sed 's/\^//2'								|
sed 's/\^//2'								|
cat

exit 0

