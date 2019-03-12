#!/bin/sh
#create units_alias

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

if [ "$application" != "hydrology" -a "$application" != "audubon" ]
then
	exit 0
fi

table_name=units_alias
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (units_alias char (15) not null,units char (15)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (units_alias);" | sql.e

exit 0
