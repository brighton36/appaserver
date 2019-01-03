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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

echo "	create view bank_upload_feeder_archive as			\
	select bank_date, bank_description, feeder_account,		\
		sequence_number, bank_amount, bank_running_balance,	\
		bank_upload_event.bank_upload_date_time			\
	from bank_upload_event, bank_upload_archive			\
	where bank_upload_event.bank_upload_date_time =			\
		bank_upload_archive.bank_upload_date_time;"		|
sql.e

exit 0

