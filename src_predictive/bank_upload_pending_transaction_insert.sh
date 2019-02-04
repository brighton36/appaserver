#!/bin/bash
# -------------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_pending_transaction_insert.sh
# -------------------------------------------------------------------------

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
	echo "\$ . set_database" 1>&2
	exit 1
fi

usage_message=0

if [ "$1" = "all" ]
then
	break_in_middle=0
elif [ "$1" = "one" ]
then
	break_in_middle=1
else
	usage_message=1
fi

if [ "$usage_message" -eq 1 ]
then
	echo "Usage: $0 all|one [process]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	echo "<h1>`echo $2 | format_initial_capital.e`</h1>"
fi

populate_bank_upload_pending.sh 			|
while read record
do
	echo $record | html_paragraph_wrapper.e

	sub_record=`echo $record | piece.e '[' 0`
	bank_date=`echo $sub_record | piece.e '^' 0`
	bank_description=`echo $sub_record | piece.e '^' 1`

	bank_upload_transaction_insert "${bank_date}^${bank_description}" |
	tee_process.e sql.e						  |
	html_paragraph_wrapper.e					  |
	cat

	if [ "$break_in_middle" -eq 1 ]
	then
		break
	fi

done

bank_upload_balance_propagate.sh bank_date bank_description | sql.e
bank_upload_sequence_propagate.sh bank_date bank_description | sql.e

exit 0
