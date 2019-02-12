#!/bin/bash
# ----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_balance_propagate.sh
# ----------------------------------------------------------------

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

select="bank_date,concat(bank_description,'^bank_running_balance'),bank_amount,bank_running_balance"

table=bank_upload

where="sequence_number >= $prior_sequence_number and exists ( select 1 from bank_upload_transaction where bank_upload.bank_date = bank_upload_transaction.bank_date and bank_upload.bank_description = bank_upload_transaction.bank_description )"

order=sequence_number

key=bank_date,bank_description

# ----------------------------------------------------------------------
# Example: 2018-01-02^SAVE MART#622S^bank_running_balance^-38.67^3455.85
# ----------------------------------------------------------------------

echo "select $select from $table where $where order by $order;"	|
sql.e								|
accumulate.e 3 '^' running					|
# --------------------
#piece out bank_amount
# --------------------
piece_inverse.e 3 '^' 						|
update_statement.e table=bank_upload key=$key carrot=y		|
cat

exit 0
