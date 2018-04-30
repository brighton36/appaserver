#!/bin/bash
# src_sparrow/update_inside_seven_minute_count.sh
# -----------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

echo Starting: $0 $* 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 ignored state quad_sheet site_number visit_date" 1>&2
	exit 1
fi

state=$2
quad_sheet=$3
site_number=$4
visit_date=$5

if [ "$state" = "predelete" ]
then
	exit 0
fi

site_visit=$(get_table_name $application site_visit)
sparrow_observation=$(get_table_name $application sparrow_observation)
where="quad_sheet='$quad_sheet' and site_number=$site_number and visit_date='$visit_date'"

bird_count=$(echo "select sum(bird_count) from $sparrow_observation where $where;" | sql.e)

if [ "$bird_count" = "" -o "$bird_count" = "0" ]
then
	reason=$(echo "select reason_no_observations from $site_visit where $where;" | sql.e)

	if [ "$reason" != "" ]
	then
		set="inside_seven_minute_count = null"
	else
		bird_count="0"
	fi
fi

if [ "$set" = "" ]
then
	set="inside_seven_minute_count = $bird_count"
fi

echo "update $site_visit set $set where $where;" | sql.e

exit 0
