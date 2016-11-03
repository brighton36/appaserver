:
# src_creel/unvalidate_fishing_trip.sh
# -----------------------------------------

echo Starting: $0 $* 1>&2

if [ "$#" -lt 6 ]
then
	echo "Usage: $0 application fishing_purpose census_date interview_location interview_number login_name" 1>&2
	exit 1
fi

application=`echo $1 | piece.e ':' 0`	# May have appended database
fishing_purpose=$2
census_date=$3
interview_location=$4
interview_number=$5
login_name=$6

table=$(get_table_name $application fishing_trips)
set="last_changed_by = '$login_name',validation_date = null"
where="fishing_purpose='$fishing_purpose' and census_date='$census_date' and interview_location='$interview_location' and interview_number='$interview_number'"

echo "update $table set $set where $where;" | sql.e

exit 0
