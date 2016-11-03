:
# src_benthic/populate_sampling_event.sh
# --------------------------------------

echo Starting: $0 $* 1>&2

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 application from_process_yn [where]" 1>&2
	exit 1
fi

application=$1
from_process_yn=$2

if [ "$#" -eq 3 -a "$3" != "where" -a "$3" != "\$where" ]
then
	parameter_where=$3
else
	parameter_where="1=1"
fi

if [ "$from_process_yn" = "y" ]
then
	where="$parameter_where and validated_date is null" 
else
	where="$parameter_where"
fi

table=$(get_table_name $application sampling_point)
select="anchor_date,anchor_time,location,concat( site_number,'|', anchor_date ), anchor_time,project,collection_name,location,site_number"
#order="anchor_date desc,anchor_time asc"
order="location,site_number"

echo "select $select from $table where $where order by $order;"	|
sql.e

exit 0
