:
echo "$0 $*" 1>&2

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 ignored full_name street_address transaction_date_time account preupdate_transaction_date_time preupdate_account" 1>&2
	exit 1
fi

full_name=$2
street_address=$3
transaction_date_time=$4
account_name=$5
preupdate_transaction_date_time=$6
preupdate_account_name=$7

if [ "$transaction_date_time" = ""				\
-o   "$transaction_date_time" = "transaction_date_time" ]
then
	exit 0
fi

if [ "$preupdate_transaction_date_time" != "preupdate_transaction_date_time" ]
then
	sys_string="echo \"select account from journal_ledger where transaction_date_time = '$transaction_date_time';\" | sql.e"

	eval $sys_string						|
	while read local_account
	do
		ledger_propagate					\
				$application				\
				"$transaction_date_time"		\
				"$preupdate_transaction_date_time"	\
				"$local_account"
	done
fi

if [ "$account_name" != "" -a "$account_name" != "account" ]
then
	ledger_propagate	$application				\
				"$transaction_date_time"		\
				"$preupdate_transaction_date_time"	\
				"$account_name"
fi

if [ "$preupdate_account_name" != ""					\
-a   "$preupdate_account_name" != "preupdate_account" ]
then
	ledger_propagate	$application				\
				"$transaction_date_time"		\
				"$preupdate_transaction_date_time"	\
				"$preupdate_account_name"
fi

if [ "$full_name" = "full_name" ]
then
	exit 0
fi

full_name_escaped=`echo $full_name | escape_character.e "'"`

from=journal_ledger
where="full_name = '$full_name_escaped' and street_address = '$street_address' and transaction_date_time = '$transaction_date_time'"

table=transaction
select="full_name,street_address,transaction_date_time,'transaction_amount'"
key="full_name,street_address,transaction_date_time"

group="group by $select"

echo "select $select,sum(debit_amount) from $from where $where $group;"	|
sql.e									|
update_statement table=$table key=$key carrot=y				|
sql.e

exit 0
