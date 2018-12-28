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

if [ "$application" != "timriley" -a	\
     "$application" != "appahost" -a	\
     "$application" != "eves" -a	\
     "$application" != "capitolpops" -a	\
     "$application" != "rudek" ]
then
	exit 0
fi

table_name=`get_table_name $application reoccurring_transaction`
echo "alter table $table_name change bank_upload_search_phrase bank_upload_feeder_phrase char (50);" | sql.e 2>&1 | grep -v 'Unknown column'

exit 0
