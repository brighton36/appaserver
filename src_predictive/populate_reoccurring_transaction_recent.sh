:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1
export DATABASE=$application

table="reoccurring_transaction"

select="full_name,street_address,transaction_description"

where="accrued_daily_amount is null"

order="$select"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
