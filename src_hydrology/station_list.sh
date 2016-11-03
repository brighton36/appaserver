#!/bin/sh
# ---------------------------------------------
# station_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application [login_name] [station] [where]" 1>&2
	exit 1
fi

application=$1

station=`get_table_name $application station`
appaserver_user_agency=`get_table_name $application appaserver_user_agency`

if [ "$#" -ge 2 -a "$2" != "login_name" ]
then

	login_where_clause="$appaserver_user_agency.login_name = '$2'"
	login_join_clause="$appaserver_user_agency.agency = $station.agency"
	from_clause="$station, $appaserver_user_agency"

else
	login_where_clause="1 = 1"
	login_join_clause="1 = 1"
	from_clause="$station"
fi

if [ "$#" -ge 3 -a "$3" != "station" ]
then
	station_and_clause="$station.station in (`single_quotes_around.e $3`)"
else
	station_and_clause="1 = 1"
fi

if [ "$#" -eq 4 -a "$4" != "$where" ]
then
	additional_where="$4"
else
	additional_where="1 = 1"
fi


echo "	select $station.station						\
	from ${from_clause}						\
	where ${login_where_clause}					\
	  and ${login_join_clause}					\
	  and ${station_and_clause}					\
	  and ${additional_where}					\
	order by $station.station;"					|
sql.e									|
toupper.e								|
cat

exit 0
