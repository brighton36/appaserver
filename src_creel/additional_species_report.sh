:
# -------------------------------------------------------
# $APPASERVER_HOME/src_creel/additional_species_report.sh
# -------------------------------------------------------

# Constants
# ---------
queue_top_bottom=500

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 10 ]
then
	echo "Usage: $0 application process_name fishing_purpose begin_date end_date family genus species detail_yn output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

fishing_purpose=$3
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	fishing_purpose="sport"		# Defaults to sport
fi

begin_date=$4
if [ "$begin_date" = "" -o "$begin_date" = "begin_date" ]
then
	begin_date="1901-01-01"
fi

end_date=$5
if [ "$end_date" = "" -o "$end_date" = "end_date" ]
then
	end_date=`now.sh ymd`
fi

family=$6
genus=$7
species=$8

detail_yn=$9

output_medium=${10}
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

# Constants
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"
process_title_half=$(echo $process_name \(${fishing_purpose}\) for ${family}/${genus}							|
format_initial_capital.e)
process_title="${process_title_half}/$species from $begin_date to $end_date"
process_title_table="${process_title_half}/$species<br>From $begin_date To $end_date"

additional_species_report_detail()
{
	fishing_purpose=$1
	begin_date=$2
	end_date=$3
	family=$4
	genus=$5
	species=$6
	output_file=$7

	heading="census_date,interview_location,interview_number,interview_time,preferred,fishing_area,day_of_week,researcher,composition,residence,trip_hours,party_count,hours_fishing,family,genus,species,kept,released"

	justification="left,left,right,left,left,left,left,left,left,left,right"

	echo "#${process_title}" > $output_file
	echo "$heading" >> $output_file

	additional_species_select_detail.sh	$fishing_purpose	\
						$begin_date		\
						$end_date		\
						"$family"		\
						"$genus"		\
						"$species"		|
	tr '^' ','							|
	group_trim.e ',' 13						|
	cat >> $output_file
}

additional_species_report_summary()
{
	fishing_purpose=$1
	begin_date=$2
	end_date=$3
	family=$4
	genus=$5
	species=$6
	output_file=$7

	heading="Fishing Count,Family,Genus,Species"

	justification="right,left,left,left"

	echo "#${process_title}" > $output_file
	echo "$heading" >> $output_file

	additional_species_select_summary.sh	$fishing_purpose	\
						$begin_date		\
						$end_date		\
						"$family"		\
						"$genus"		\
						"$species"		|
	tr '^' ','							|
	tr '|' ','							|
	cat >> $output_file
}

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
	echo "<body><h1>$process_title_table</h1>"
fi

if [ "$detail_yn" = "y" ]
then
	additional_species_report_detail	$fishing_purpose	\
						$begin_date		\
						$end_date		\
						$family			\
						$genus			\
						$species		\
						$output_file
else
	additional_species_report_summary	$fishing_purpose	\
						$begin_date		\
						$end_date		\
						$family			\
						$genus			\
						$species		\
						$output_file
fi

if [ "$output_medium" = "text_file" -o "$output_medium" = "spreadsheet" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
elif [ "$output_medium" = "table" ]
then
	cat $output_file				|
	grep -v "$heading"				|
	grep -v "^#"					|
	queue_top_bottom_lines.e $queue_top_bottom	|
	html_table.e "" "$heading" "," "$justification"
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit 0

