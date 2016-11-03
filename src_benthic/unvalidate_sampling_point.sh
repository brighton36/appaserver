:
# src_benthic/unvalidate_sampling_point.sh
# -----------------------------------------

echo Starting: $0 $* 1>&2

if [ "$#" -lt 5 ]
then
	echo "Usage: $0 application anchor_date anchor_time location site_number" 1>&2
	exit 1
fi

application=$1
anchor_date=$2
anchor_time=$3
location=$4
site_number=$5

table=$(get_table_name $application sampling_point)
set="validated_login_name = null,validated_date = null, validated_time = null"
where="anchor_date='$anchor_date' and anchor_time='$anchor_time' and location='$location' and site_number='$site_number'"

echo "update $table set $set where $where;" | sql.e

exit 0
