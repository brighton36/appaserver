#!/bin/sh
#create activity

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

folder_attribute_exists.sh $application bank_uploads bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

if [	"$application" != "eves"		\
-a	"$application" != "appahost"		\
-a	"$application" != "timriley" ]
then
	table_name=activity
	echo "drop table if exists $table_name;" | sql.e
	echo "create table $table_name (activity char (30) not null) engine MyISAM;" | sql.e
	echo "create unique index $table_name on $table_name (activity);" | sql.e
fi

table_name=role_activity
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (role char (25) not null,activity char (30) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (role,activity);" | sql.e


table_name=bank_upload_activity_work
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (login_name char (50) not null,begin_work_date_time datetime not null,end_work_date_time datetime,bank_upload_date_time datetime,role char (25),activity char (30),work_hours double (6,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (login_name,begin_work_date_time);" | sql.e


table_name=bank_upload_archive
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (bank_date date not null,bank_description char (140) not null,sequence_number integer,bank_amount double (10,2),bank_running_balance double (12,2),bank_upload_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_date,bank_description);" | sql.e


table_name=bank_upload_event
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (bank_upload_date_time datetime not null,bank_upload_filename char (80),completed_date_time datetime) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (bank_upload_date_time);" | sql.e

exit 0
