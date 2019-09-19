#!/bin/sh
#create favorite_station

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

table_name=favorite_station
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (favorite_station_set_name char (50) not null,login_name char (20) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (favorite_station_set_name,login_name);" | sql.e

table_name=favorite_station_member
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (favorite_station_set_name char (50) not null,login_name char (20) not null,station char (20) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (favorite_station_set_name,login_name,station);" | sql.e

