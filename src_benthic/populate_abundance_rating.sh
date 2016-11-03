:
if [ "$#" -lt 2 ]
then
	echo "Usage: $0 application [one2m_related_folder]" 1>&2
	exit 1
fi

application=$1

if [ "$#" -eq 2 ]
then
	one2m_related_folder=$2
fi

if [ "$one2m_related_folder" = "quad_vegetation" ]
then
	where="abundance_rating != '0 = absent'"
else
	where="1 = 1"
fi

table=$(get_table_name $application abundance_rating)
select="abundance_rating"

echo "select $select from $table where $where;" | sql.e | sort -n

exit 0
