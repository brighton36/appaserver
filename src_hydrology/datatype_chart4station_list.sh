#!/bin/sh
# ---------------------------------------------
# datatype_chart4station_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 application login_name [station]" 1>&2
	exit 1
fi

application=$1
login_name=$2

station_table=`get_table_name $application station`
station_datatype=`get_table_name $application station_datatype`
appaserver_user_agency=`get_table_name $application appaserver_user_agency`

if [ "$#" -eq 3 ]
then
	station_and_clause="and $station_datatype.station = '$3'"
else
	station_and_clause="and 1 = 1"
fi

echo "		select $station_datatype.datatype			\
		from 	$station_table,					\
			$station_datatype,				\
			$appaserver_user_agency				\
		where	$station_table.agency =				\
			$appaserver_user_agency.agency			\
		  and $station_table.station = $station_datatype.station\
		  and $appaserver_user_agency.login_name = '$login_name'\
		  $station_and_clause					\
		  and $station_datatype.datatype <> 'voltage'		\
		  and $station_datatype.plot_for_station_check_yn = 'y'	\
		order by $station_datatype.datatype;"			|
sql									|
cat
