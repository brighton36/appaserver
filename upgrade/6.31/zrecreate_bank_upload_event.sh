#!/bin/sh
#create bank_upload_event

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

set -x

table_name=bank_upload_event
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (bank_upload_date_time datetime not null,login_name char (50),completed_date_time datetime,bank_upload_filename char (80),file_sha256sum char (64)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_upload_date_time);" | sql.e
echo "create unique index ${table_name}_additional_unique on $table_name (file_sha256sum);" | sql.e


