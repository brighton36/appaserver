#!/bin/sh
# ------------------------------------------------------------
# station_datatype_agency_list.sh
# ------------------------------------------------------------
# This appends the datatype's units.
# Freely available software: see Appaserver.org
# ------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 9 ]
then
	echo "Usage: $0 application login_name process station datatype plot_for_station_check_yn dont_filter_manipulate_yn validation_required_yn filter_calibrated_yn [favorite_station_set_name]" 1>&2
	exit 1
fi

application=$1
login_name=$2
process_name=$3

got_station=0

if [ "$4" != "station" ]
then
	station_and_clause="and (station_datatype.station in (`single_quotes_around.e $4`)"

	if [ "$process_name" != "station_datatype_list" ]
	then
		got_station=1
	fi
else
	station_and_clause="and (1 = 1"
fi

if [ "$5" != "datatype" ]
then
	datatype_and_clause="and station_datatype.datatype in (`single_quotes_around.e $5`))"
else
	datatype_and_clause="and 1 = 1)"
fi

if [ "$6" != "plot_for_station_check_yn" ]
then
	plot_for_station_check_yn_and_clause="and station_datatype.plot_for_station_check_yn = '$6'"
else
	plot_for_station_check_yn_and_clause="and 1 = 1"
fi

if [ "$7" = "y" ]
then
	dont_filter_manipulate_yn="y"
fi

if [ "$8" != "validation_required_yn" ]
then
	validation_required_yn_and_clause="and station_datatype.validation_required_yn = '$8'"
else
	validation_required_yn_and_clause="and 1 = 1"
fi

if [ "$9" = "y" ]
then
	calibrated_yn_and_clause="and datatype.calibrated_yn = 'y'"
else
	calibrated_yn_and_clause="and 1 = 1"
fi

favorite_station_set_name=""

if [ "$#" -eq 10 ]
then
	favorite_station_set_name="${10}"

	if [ "$favorite_station_set_name" = "favorite_station_set_name" ]
	then
		favorite_station_set_name=""
	fi
fi

from="datatype,station,station_datatype,appaserver_user_agency"

if [ "$favorite_station_set_name" != "" ]
then
	from="datatype,station,station_datatype,appaserver_user_agency,favorite_station_member"

	escaped=`echo "$favorite_station_set_name" | escape_character.e "'"`

	favorite_station_where="favorite_station_set_name = '$escaped' and favorite_station_member.station = station.station and favorite_station_member.login_name = '$login_name'"

else
	favorite_station_where="1 = 1"
fi

process_group=`echo "	select process_group
			from process
			where process = '$process_name';" |
	       sql.e`

if [ "$dont_filter_manipulate_yn" != "y" -a "$process_group" = "manipulate" ]
then
	agency=`echo "	select agency
			from appaserver_user_agency
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

# Note: station_datatype_list.c parses this.
# ------------------------------------------
select="	station.station,					\
		station_datatype.datatype,				\
		concat(	' [',						\
			ifnull(datatype.units,''),			\
			'--',						\
			ifnull(station.agency,''),			\
			']' )"

echo "	select $select							\
	from $from							\
	where station.agency = appaserver_user_agency.agency		\
	  and station.station = station_datatype.station		\
	  and appaserver_user_agency.login_name = '$login_name'		\
	  and station_datatype.datatype = datatype.datatype		\
	  and $favorite_station_where					\
	  $station_and_clause						\
	  $datatype_and_clause						\
	  $plot_for_station_check_yn_and_clause				\
	  $validation_required_yn_and_clause				\
	  $calibrated_yn_and_clause					\
	  $station_datatype_and_clause					\
	order by station.station, station_datatype.datatype;"		|
sql.e '^'								|
sed 's/\^//2'								|
cat

exit 0

