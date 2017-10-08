#!/bin/sh
#create foreign_attribute

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

table_name=`get_table_name $application foreign_attribute`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (folder char (35) not null,related_folder char (35) not null,related_attribute char (60) not null,foreign_attribute char (60) not null,primary_key_index integer) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (folder,related_folder,related_attribute,foreign_attribute);" | sql.e '^' mysql

