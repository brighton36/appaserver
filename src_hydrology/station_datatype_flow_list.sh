#!/bin/sh
# ------------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/station_datatype_flow_list.sh
# ------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application login_name role" 1>&2
	exit 1
fi

application=$1
login_name=$2
role_name=$3

station_table=`get_table_name $application station`
station_datatype=`get_table_name $application station_datatype`
appaserver_user_agency=`get_table_name $application appaserver_user_agency`
role_table=`get_table_name $application role`

datatype_and_clause="and $station_datatype.datatype like 'flow%'"

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
	#validation_and_clause="and validation_required_yn = 'y'"
#else
	validation_and_clause="and 1 = 1"
#fi

echo "	select $station_table.station, $station_datatype.datatype	\
	from $station_table, $station_datatype, $appaserver_user_agency	\
	where $station_table.agency = $appaserver_user_agency.agency	\
	  and $station_table.station = $station_datatype.station	\
	  and $appaserver_user_agency.login_name = '$login_name'	\
	  $datatype_and_clause						\
	  $validation_and_clause					\
	order by $station_table.station, $station_datatype.datatype;"	|
sql '^'									|
cat

exit 0

