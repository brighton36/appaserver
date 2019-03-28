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

echo "delete from folder_attribute where folder = 'datatype' and attribute = 'ysi_load_heading';"						|
sql.e

echo "delete from folder_attribute where folder = 'datatype' and attribute = 'exo_load_heading';"						|
sql.e

field="datatype,datatype_alias"

echo "select datatype,ysi_load_heading from datatype where ysi_load_heading is not null;"							|
sql.e								|
insert_statement.e table=datatype_alias field=$field del='^'	|
sql.e 2>&1							|
grep -vi duplicate

echo "select datatype,exo_load_heading from datatype where exo_load_heading is not null;"							|
sql.e								|
insert_statement.e table=datatype_alias field=$field del='^'	|
sql.e 2>&1							|
grep -vi duplicate

exit 0
