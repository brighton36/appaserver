:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1
export DATABASE=$application

table="entity"

select="full_name,street_address"

where="exists ( select 1 from reoccurring_transaction where entity.full_name = reoccurring_transaction.full_name and entity.street_address = reoccurring_transaction.street_address )"

order="$select"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
