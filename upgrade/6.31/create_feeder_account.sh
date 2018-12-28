#!/bin/sh
#create feeder_account

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

if [ "$application" != "timriley" -a	\
     "$application" != "appahost" -a	\
     "$application" != "eves" -a	\
     "$application" != "capitolpops" -a	\
     "$application" != "rudek" ]
then
	exit 0
fi

table_name=feeder_account
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (feeder_account char (40) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (feeder_account);" | sql.e

exit 0
