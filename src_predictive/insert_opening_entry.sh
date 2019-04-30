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

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 input_file fund" 1>&2
	exit 1
fi

input_file=$1
fund=$2

if [	"$input_file" = "predictivebooks_autorepair.dat" -o	\
	"$input_file" = "predictivebooks_enterprise.dat" -o	\
	"$input_file" = "predictivebooks_rentalproperty.dat" 	]
then
	equity_subclassification="contributed_capital"
else
	equity_subclassification="net_assets"
fi

# Set SELF_FULL_NAME and SELF_STREET_ADDRESS
# ------------------------------------------
. set_self.sh

now_date_time=`now.sh 19`
now_date=`now.sh ymd`

# ACCOUNT
# -------
if [ "$fund" = "" -o "$fund" = "fund" ]
then

# If not fund
# -----------
echo "insert into account ( account, subclassification, hard_coded_account_key ) values ( +place1+, +place2+, +place3+ );"			|
sed "s/place1/\$cash_account_name/"				|
sed "s/place2/cash/"						|
sed "s/place3/cash_key/"					|
sed "s/+/'/g"							|
cat

echo "insert into account ( account, subclassification, hard_coded_account_key ) values ( +place1+, +place2+, +place3+ );"			|
sed "s/place1/\$equity_account_name/"				|
sed "s/place2/$equity_subclassification/"			|
sed "s/place3/closing_key,contributed_capital_key/"		|
sed "s/+/'/g"							|
cat

else

# If fund
# -------
echo "insert into account ( account, subclassification, hard_coded_account_key, fund ) values ( +place1+, +place2+, +place3+, +place4+ );"	|
sed "s/place1/\$cash_account_name/"				|
sed "s/place2/cash/"						|
sed "s/place3/cash_key/"					|
sed "s/place4/$fund/"						|
sed "s/+/'/g"							|
cat

echo "insert into account ( account, subclassification, hard_coded_account_key, fund ) values ( +place1+, +place2+, +place3+, +place4+ );"	|
sed "s/place1/\$equity_account_name/"				|
sed "s/place2/$equity_subclassification/"			|
sed "s/place3/closing_key/"					|
sed "s/place4/$fund/"						|
sed "s/+/'/g"							|
cat

fi

# TRANSACTION
# -----------
echo "insert into transaction ( full_name, street_address, transaction_date_time, transaction_amount, memo ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ );"							|
sed "s/place1/$SELF_FULL_NAME/"					|
sed "s/place2/$SELF_STREET_ADDRESS/"				|
sed "s/place3/$now_date_time/"					|
sed "s/place4/\$cash_opening_balance/"				|
sed "s/place5/Opening entry/"					|
sed "s/+/'/g"							|
cat

# JOURNAL_LEDGER (debit)
# ----------------------
echo "insert into journal_ledger ( full_name, street_address, transaction_date_time, account, debit_amount ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ );"							|
sed "s/place1/$SELF_FULL_NAME/"					|
sed "s/place2/$SELF_STREET_ADDRESS/"				|
sed "s/place3/$now_date_time/"					|
sed "s/place4/\$cash_account_name/"				|
sed "s/place5/\$cash_opening_balance/"				|
sed "s/+/'/g"							|
cat

# JOURNAL_LEDGER (credit)
# -----------------------
echo "insert into journal_ledger ( full_name, street_address, transaction_date_time, account, credit_amount ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ );"							|
sed "s/place1/$SELF_FULL_NAME/"					|
sed "s/place2/$SELF_STREET_ADDRESS/"				|
sed "s/place3/$now_date_time/"					|
sed "s/place4/\$equity_account_name/"				|
sed "s/place5/\$cash_opening_balance/"				|
sed "s/+/'/g"							|
cat

# BANK_UPLOAD
# -----------
echo "insert into bank_upload ( bank_date, bank_description, sequence_number, bank_running_balance ) values ( +place1+, +place2+, +place3+, +place4+ );"									|
sed "s/place1/$now_date/"					|
sed "s/place2/Opening balance/"					|
sed "s/place3/1/"						|
sed "s/place4/\$cash_opening_balance/"				|
sed "s/+/'/g"							|
cat

# BANK_UPLOAD_TRANSACTION
# -----------------------
echo "insert into bank_upload_transaction ( bank_date, bank_description, full_name, street_address, transaction_date_time ) values ( +place1+, +place2+, +place3+, +place4+, +place5+ );"					|
sed "s/place1/$now_date/"					|
sed "s/place2/Opening balance/"					|
sed "s/place3/$SELF_FULL_NAME/"					|
sed "s/place4/$SELF_STREET_ADDRESS/"				|
sed "s/place5/$now_date_time/"					|
sed "s/+/'/g"							|
cat

exit 0
