#!/bin/bash
# -------------------------------------------------------
# $APPASERVER_HOME/src_predictive/insert_opening_entry.sh
# -------------------------------------------------------

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

# Set SELF_FULL_NAME and SELF_STREET_ADDRESS
# ------------------------------------------
. set_self.sh

now_date_time=`now.sh 19`
now_date=`now.sh ymd`

# TRANSACTION
# -----------
echo 'echo "insert into transaction ( full_name, street_address, transaction_date_time, transaction_amount, memo ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ )"'							|
sed "s/place1/$SELF_FULL_NAME/"					|
sed "s/place2/$SELF_STREET_ADDRESS/"				|
sed "s/place3/$now_date_time/"					|
sed "s/place4/\$cash_opening_balance/"				|
sed "s/place5/Opening entry/"					|
sed "s/+/'/g"							|
cat

# JOURNAL_LEDGER (debit)
# ----------------------
echo 'echo "insert into journal_ledger ( full_name, street_address, transaction_date_time, account, debit_amount ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ )"'						|
sed "s/place1/$SELF_FULL_NAME/"					|
sed "s/place2/$SELF_STREET_ADDRESS/"				|
sed "s/place3/$now_date_time/"					|
sed "s/place4/\$cash_account_name/"				|
sed "s/place5/\$cash_opening_balance/"				|
sed "s/+/'/g"							|
cat

# JOURNAL_LEDGER (credit)
# -----------------------
echo 'echo "insert into journal_ledger ( full_name, street_address, transaction_date_time, account, credit_amount ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ )"'						|
sed "s/place1/$SELF_FULL_NAME/"					|
sed "s/place2/$SELF_STREET_ADDRESS/"				|
sed "s/place3/$now_date_time/"					|
sed "s/place4/\$equity_account_name/"				|
sed "s/place5/\$cash_opening_balance/"				|
sed "s/+/'/g"							|
cat

# BANK_UPLOAD
# -----------
echo 'echo "insert into bank_upload ( bank_date, bank_description, sequence_number, bank_running_balance ) values ( +place1+, +place2+, +place3+, +place4+ )"'									|
sed "s/place1/$now_date/"					|
sed "s/place2/Opening balance/"					|
sed "s/place3/1/"						|
sed "s/place4/\$cash_opening_balance/"				|
sed "s/+/'/g"							|
cat

# BANK_UPLOAD_TRANSACTION
# -----------------------
echo 'echo "insert into bank_upload_transaction ( bank_date, bank_description, full_name, street_address, transaction_date_time ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ )"'					|
sed "s/place1/$now_date/"					|
sed "s/place2/Opening balance/"					|
sed "s/place3/$SELF_FULL_NAME/"					|
sed "s/place4/$SELF_STREET_ADDRESS/"				|
sed "s/place5/$now_date_time/"					|
sed "s/+/'/g"							|
cat

exit 0
