#!/bin/sh
# ---------------------------------------------
# station_datatype_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

#if [ "$#" -lt 3 ]
#then
	#echo "Usage: $0 application login_name role [station] [datatype]" 1>&2
	#exit 1
#fi

application=everglades
login_name=tim
#role_name=$3

station_table=`get_table_name $application station`
station_datatype=`get_table_name $application station_datatype`
appaserver_user_agency=`get_table_name $application appaserver_user_agency`
#role_table=`get_table_name $application role`

#if [ "$#" -ge 4 -a "$4" != "station" ]
#then
	station_and_clause="and $station_datatype.station = 'TAMI_Q1'"
#else
	#station_and_clause="and 1 = 1"
#fi

#if [ "$#" -ge 5 -a "$5" != "datatype" ]
#then
	#datatype_and_clause="and $station_datatype.datatype = '$5'"
#else
	datatype_and_clause="and 1 = 1"
#fi

#override_row_restrictions_yn=`						 \
#	echo "	select override_row_restrictions_yn			 \
#		from $role_table					 \
#		where $role_table.role = '$role_name';"			|\
#	sql`
#
#if [ "$override_row_restrictions_yn" = 'y' ]
#then
#	validation_and_clause="and 1 = 1"
#else
#	validation_and_clause="and validation_required_yn = 'y'"
#fi

#if [ "$role_name" = public ]
#then
#	validation_and_clause="and validation_required_yn = 'y'"
#else
	validation_and_clause="and 1 = 1"
#fi

set -x
echo "	select $station_table.station, $station_datatype.datatype	\
	from $station_table, $station_datatype				\
	where $station_table.station = $station_datatype.station	\
	  $station_and_clause						\
	  $datatype_and_clause						\
	order by $station_table.station, $station_datatype.datatype;"	|
sql '|'									|
cat

exit 0

