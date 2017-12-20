#!/bin/sh
#create subsidiary_transaction

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != timriley -a		\
     "$applicaiion" != eves -a			\
     "$application" != appahost ]
then
	exit 0
fi

table_name=`get_table_name $application subsidiary_transaction`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (folder char (35) not null,attribute char (60),debit_account char (60),debit_account_folder char (50),credit_account char (60)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (folder);" | sql.e '^' mysql


