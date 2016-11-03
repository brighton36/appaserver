:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application one2m_folder" 1>&2
	exit 1
fi

application=$1
one2m_folder=$2

export DATABASE=$application

table="fund_account"

select="concat( fund, '^', account, '|', account, '^', fund )"

if [ "$one2m_folder" = "donation_program" ]
then
	where="account in ( select account from account where subclassification = 'donation' )"
else
	where="1 = 1"
fi

order="$select"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
