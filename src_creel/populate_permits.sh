:

echo Starting: $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application state" 1>&2
	exit 1
fi

application=$1
state=$2

select="permit_code"

if [ "$state" = "insert" ]
then
	where="expiration_date is null"
else
	where="1 = 1"
fi

echo "select $select from permits where $where order by $select;"	|
sql.e

exit 0
