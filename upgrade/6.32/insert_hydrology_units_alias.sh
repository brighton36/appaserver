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

if [ "$application" != "hydrology" -a "$application" != "audubon" ]
then
	exit 0
fi

table_name=units_alias
(
sql.e << all_done
insert into $table_name (units_alias,units) values ('PSU','ppt');
insert into $table_name (units_alias,units) values ('µS/cm','uS/cm');
insert into $table_name (units_alias,units) values ('c','celsius');
insert into $table_name (units_alias,units) values ('ft','feet');
all_done
) 2>&1 | grep -vi duplicate
exit 0
