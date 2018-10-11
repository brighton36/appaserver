#!/bin/sh
#create automatic_load_method

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

table_name=automatic_load_method
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (automatic_load_method char (25) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (automatic_load_method);" | sql.e

echo "insert into automatic_load_method values ( 'ASCII_station' );" | sql.e
echo "insert into automatic_load_method values ( 'binary_station' );" | sql.e
echo "insert into automatic_load_method values ( 'shef_station' );" | sql.e
echo "insert into automatic_load_method values ( 'pseudo_binary_station' );" | sql.e

exit 0
