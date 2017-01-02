:
# src_benthic/species_most_frequently_caught.sh
# ---------------------------------------------

# Input
# -----
if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application process_name caught_more_than group_time output_medium" 1>&2
	exit 1
fi

echo $0 $* 1>&2

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

caught_more_than=$3
if [ "$caught_more_than" = "" -o "$caught_more_than" = "caught_more_than" ]
then
	caught_more_than=1		# Defaults to 1
fi

group_time=$4
if [ "$group_time" = "" -o "$group_time" = "group_time" ]
then
	group_time="annual"		# Defaults to annual
fi

output_medium=$5
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

# System variables
# ----------------
document_root_directory=`get_document_root.e`
output_file="$document_root_directory/appaserver/$application/data/${process_name}_$$.csv"
row_file="/tmp/row_${process_name}_$$"
denominator_file="/tmp/denominator_${process_name}_$$"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"
exit_flag=0

# User variables
# --------------
species_count=`get_table_name $application species_count`
benthic_species=`get_table_name $application benthic_species`
process_title=$(echo $process_name | format_initial_capital.e)

# Preprocess
# ----------
if [ "$group_time" = "annual" ]
then
	first_select="substr( anchor_date, 1, 4 )"
	heading="Year,Common,Family,Genus,Species,Caught,Denominator,Percent"
elif [ "$group_time" = "year_month" ]
then
	first_select="substr( anchor_date, 1, 7 )"
	heading="Year-Month,Common,Family,Genus,Species,Caught,Denominator,Percent"
elif [ "$group_time" = "month" ]
then
	first_select="substr( anchor_date, 6, 2 )"
	heading="Month,Common,Family,Genus,Species,Caught,Denominator,Percent"
fi

justification="left,left,left,left,left,right,right,right"

if [ "$output_medium" = "text_file" -o "$output_medium" = "spreadsheet" ]
then
	title_maybe=$process_title
	heading_maybe=$heading
elif [ "$output_medium" = "table" ]
then
	title_maybe=""
	heading_maybe=""
elif [ "$output_medium" = "stdout" ]
then
	title_maybe="#$process_title"
	heading_maybe="#$heading"
fi

# Process
# -------
echo "select $first_select group_by, sum( count_per_square_meter ) caught
from $species_count
where count_per_square_meter > 0
group by group_by
having caught >= $caught_more_than;"			|
sql.e ',' > $denominator_file

echo "select $first_select group_by, common_name, $species_count.family_name, $species_count.genus_name, $species_count.species_name, sum( count_per_square_meter ) caught
from $species_count, $benthic_species
where $species_count.family_name = $benthic_species.family_name
  and $species_count.genus_name = $benthic_species.genus_name
  and $species_count.species_name = $benthic_species.species_name
  and count_per_square_meter > 0
group by group_by, $species_count.family_name, $species_count.genus_name, $species_count.species_name
having caught >= $caught_more_than
order by group_by, caught desc;"			|
sql.e ',' > $row_file

(
echo "$title_maybe"
echo "$heading_maybe"
join -t',' -a1 $row_file $denominator_file		|
awk 'BEGIN {
	FS = ","
}
{
	year_month = $1
	common_name = $2
	family_name = $3
	genus_name = $4
	species_name = $5
	count_per_square_meter = $6
	denominator = $7
	results = ( count_per_square_meter / denominator ) * 100
	printf ( "%s,%s,%s,%s,%s,%d,%d,%.3f\n",
		year_month,
		common_name,
		family_name,
		genus_name,
		species_name,
		count_per_square_meter,
		denominator,
		results )
}'
) | cat > $output_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$output_medium" = "text_file" -o "$output_medium" = "spreadsheet" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
elif [ "$output_medium" = "table" ]
then
	cat $output_file				|
	html_table.e "" "$heading" "," "$justification"
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
else
	echo "Error: invalid output medium of $output_medium" 1>&2
	exit_flag=1
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

rm -f $row_file
rm -f $denominator_file

exit $exit_flag

