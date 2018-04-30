:
if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ignored" 1>&2
	exit 1
fi

table="reoccurring_transaction"

select="full_name,street_address,transaction_description"

where="accrued_daily_amount is null"

order="$select"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
