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
	prior_sequence_number=`	bank_upload_prior_sequence_number.sh
				"$bank_date"`
fi

select="bank_upload.bank_date,bank_upload.bank_description"

table="bank_upload,bank_upload_transaction"

join_where="bank_upload.bank_date = bank_upload_transaction.bank_date and bank_upload.bank_description = bank_upload_transaction.bank_description"

where="sequence_number >= $prior_sequence_number and $join_where"

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
