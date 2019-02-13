#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_sequence_propagate.sh
# -----------------------------------------------------------------

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

echo "$0 $*" 1>&2

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 bank_date" 1>&2
	exit 1
fi

# May have time appended
# ----------------------
bank_date=`echo $1 | column.e 0`

if [ "$bank_date" = "" -o "$bank_date" = "bank_date" ]
then
	prior_sequence_number=1
else
	# Returns prior_sequence_number^transaction_date_time
	# ---------------------------------------------------
	results=`bank_upload_prior_sequence_number.sh "$bank_date"`
	prior_sequence_number=`echo $results | piece.e '^' 0`
	prior_transaction_date_time=`echo $results | piece.e '^' 1`
fi

select="bank_date,bank_description"

table="bank_upload_transaction_balance"

# Start with the next transaction_date_time
# -----------------------------------------
where="transaction_date_time > '$prior_transaction_date_time'"

order=transaction_date_time

# ----------------------------------------------------------------------
# Note: when doing the period of record, the opening row in BANK_UPLOAD
# isn't joined to BANK_UPLOAD_TRANSACTION. Usually, it's sequence_number
# is 1. So, start setting the new sequence numbers to at least 2.
# ----------------------------------------------------------------------

let "new_sequence_number = $prior_sequence_number + 1"

(
echo "select $select from $table where $where order by $order;"	|
sql.e								|
while read record
do
	echo "${record}^sequence_number^$new_sequence_number"
	let new_sequence_number++
done
)								|
update_statement.e table=bank_upload key=$select carrot=y

exit 0
