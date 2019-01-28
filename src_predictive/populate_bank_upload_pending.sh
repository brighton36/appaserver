#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_bank_upload_pending.sh
# ---------------------------------------------------------------

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

select="concat( bank_date, '^',bank_description, ' [', bank_amount, ']' )"

order="sequence_number"

table=bank_upload

where="not exists (
		select 1						\
		from bank_upload_transaction				\
		where bank_upload.bank_date =				\
			bank_upload_transaction.bank_date and		\
		      bank_upload.bank_description =			\
			bank_upload_transaction.bank_description )"

echo "select $select from $table where $where order by $order;"		|
sql.e									|
# --------------------------
# Why is there a blank line?
# --------------------------
grep -v '^$'								|
cat

exit 0
