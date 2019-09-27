#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/automatic_transaction_assign.sh
# ---------------------------------------------------------------

echo "$0 $*" 1>&2

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

if [ "$#" -ne 3 ]
then
	usage_message=1
fi

if [ "$usage_message" -eq 1 ]
then
	echo "Usage: $0 all|one process_name fund" 1>&2
	echo "Note: leave process_name blank for output-free execution." 1>&2
	exit 1
fi

operation=$1
process_name=$2
fund=$3

if [ "$process_name" != "" -a "$process_name" != "process_name" ]
then
	any_output=1
else
	any_output=0
fi

if [ "$any_output" -eq 1 ]
then
	echo "<html>"
	echo "<body>"
	echo "<h1>`echo $process_name | format_initial_capital.e`</h1>"
fi

first_bank_date=""

populate_bank_upload_pending.sh 			|
while read record
do
	if [ "$any_output" -eq 1 ]
	then
		echo $record | html_paragraph_wrapper.e
	fi

	sub_record=`echo $record | piece.e '[' 0`
	bank_date=`echo $sub_record | piece.e '^' 0`

	if [ "$first_bank_date" = "" ]
	then
		first_bank_date=$bank_date
	fi

	bank_description=`echo $sub_record | piece.e '^' 1`

	# Doesn't do the propagate.
	# -------------------------
	bank_upload_transaction_insert	"${bank_date}^${bank_description}" \
					"$fund" 			   |
	tee_process.e sql.e						   |
	html_paragraph_wrapper.e					   |
	cat

	if [ "$break_in_middle" -eq 1 ]
	then
		break
	fi

done

if [ "$first_bank_date" != "" ]
then
	bank_upload_sequence_propagate.sh "$first_bank_date" | sql.e
	bank_upload_balance_propagate.sh "$first_bank_date" | sql.e
fi

if [ "$any_output" -eq 1 ]
then
	echo "</body>"
	echo "</html>"
fi

exit 0
