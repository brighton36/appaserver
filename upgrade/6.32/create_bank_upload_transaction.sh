#!/bin/sh
#create bank_upload_transaction

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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

table_name=bank_upload_transaction
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (bank_date date not null,bank_description char (140) not null,full_name char (60) not null,street_address char (40) not null,transaction_date_time datetime not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_date,bank_description,full_name,street_address,transaction_date_time);" | sql.e
echo "create index ${table_name}_full_name on $table_name (full_name);" | sql.e
echo "create index ${table_name}_street_address on $table_name (street_address);" | sql.e
echo "create index ${table_name}_transaction_date_time on $table_name (transaction_date_time);" | sql.e


