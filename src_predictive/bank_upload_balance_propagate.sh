#!/bin/bash
# ----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_balance_propagate.sh
# ----------------------------------------------------------------

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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 bank_date" 1>&2
	exit 1
fi

bank_date=$1

select="min(sequence_number)"
table=bank_upload

#where="bank_date < '$bank_date'"
where="1 = 1"

in_balance_sequence_number=`
echo "select $select from $table where $where"			|
sql.e								|
cat`

if [ "$in_balance_sequence_number" = "" ]
then
	exit 0
fi

select=bank_date,bank_description,bank_amount,bank_running_balance
table=bank_upload
where="sequence_number >= $in_balance_sequence_number and exists ( select 1 from bank_upload_transaction where bank_upload.bank_date = bank_upload_transaction.bank_date and bank_upload.bank_description = bank_upload_transaction.bank_description )"
order=sequence_number
key=bank_date,bank_description

first_time=1

(
echo "select $select from $table where $where order by $order;"	|
sql.e								|
while read record
do
	if [ $first_time -eq 1 ]
	then
		bank_running_balance=`echo $record | piece.e '^' 3`
		first_time=0
		continue
	fi

	bank_date=`echo "$record" | piece.e '^' 0`
	bank_description=`echo "$record" | piece.e '^' 1`
	bank_amount=`echo "$record" | piece.e '^' 2`

	temp=$bank_running_balance
	bank_running_balance=$(echo "scale=2; $temp + $bank_amount" | bc)

	echo "${bank_date}^${bank_description}^bank_running_balance^$bank_running_balance"

done
)								|
update_statement.e table=bank_upload key=$key carrot=y		|
cat

exit 0
