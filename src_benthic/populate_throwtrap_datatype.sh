:

echo $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application state" 1>&2
	exit 1
fi

application=$1
state=$2

from="throwtrap_datatype,datatype"
join_where="throwtrap_datatype.datatype = datatype.datatype"

if [ "$state" = "insert" ]
then
	where="$join_where and omit_insert_yn is null or omit_insert_yn = 'n'"
else
	where="$join_where"
fi

select="throwtrap_datatype.datatype"
order="display_order"

echo "select $select from $from where $where order by $order;" | sql.e

exit 0
