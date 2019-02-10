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
	echo "Usage: $0 bank_date [bank_description]" 1>&2
	exit 1
fi

# May have time appended
# ----------------------
bank_date=`echo $1 | column.e 0`

if [ "$#" -eq 1 ]
then
	bank_description=$2
else
	bank_description=""
fi

select=sequence_number,transaction_date_time
table=bank_upload_transaction_balance

#where="bank_date < '$bank_date'"
where="1 = 1"

order=transaction_date_time

out_of_sequence_row_number=`
echo "select $select from $table where $where order by $order;"	|
sql.e								|
sort -n -c 2>&1							|
piece.e ':' 2`

if [ "$out_of_sequence_row_number" = "" ]
then
	exit 0
fi

let "in_sequence_row_number=$out_of_sequence_row_number - 1"

in_sequence_transaction_date_time=`
echo "select $select from $table order by $order;"		|
sql.e								|
head -$in_sequence_row_number					|
tail -1								|
piece.e '^' 1`

good_sequence_number=`
echo "select $select from $table order by $order;"		|
sql.e								|
head -$in_sequence_row_number					|
tail -1								|
piece.e '^' 0`

select=bank_date,bank_description
where="transaction_date_time >= '$in_sequence_transaction_date_time'"
order=transaction_date_time

(
echo "select $select from $table where $where order by $order;"	|
sql.e								|
while read record
do
	echo "${record}^sequence_number^$good_sequence_number"
	let good_sequence_number++
done
)								|
update_statement.e table=bank_upload key=$select carrot=y

exit 0
