#!/bin/sh
if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "$ . set_database" 1>&2
	exit 1
fi

if [ "$application" != hydrology -a "$application" != audubon ]
then
	exit 0
fi

table_name=`get_table_name $application crontab_load_method`
echo "alter table $table_name change automatic_load_method crontab_load_method char (25) not null;" | sql.e 2>&1 | grep -v 'Unknown column'

table_name=`get_table_name $application enp_station`
echo "alter table $table_name change automatic_load_method crontab_load_method char (25);" | sql.e 2>&1 | grep -v 'Unknown column'

exit 0
