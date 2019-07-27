:
# src_creel/species_most_frequently_caught.sh
# -------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 13 ]
then
	echo "Usage: $0 application process_name begin_year_month end_year_month fishing_purpose caught_more_than aggregate_level by_area_yn caught_kept_preferred family genus species output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

begin_year_month=$3
if [ "$begin_year_month" = "" -o "$begin_year_month" = "begin_year_month" ]
then
	begin_year_month="1958-01"
fi

end_year_month=$4
if [ "$end_year_month" = "" -o "$end_year_month" = "end_year_month" ]
then
	end_year_month=`date.e 0 | piece_inverse.e 2 '-'`
fi

fishing_purpose=$5
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	# fishing_purpose="sport"		# Defaults to sport
	fishing_purpose=""
fi

caught_more_than=$6
if [ "$caught_more_than" = "" -o "$caught_more_than" = "caught_more_than" ]
then
	caught_more_than=1		# Defaults to 1
fi

aggregate_level=$7
if [ "$aggregate_level" = "" -o "$aggregate_level" = "aggregate_level" ]
then
	aggregate_level="annually"
fi

by_area_yn=$8

caught_kept_preferred=$9
if [	"$caught_kept_preferred" = "" -o \
	"$caught_kept_preferred" = "caught_kept_preferred" ]
then
	caught_kept_preferred="caught"		# Defaults to caught
fi

family=${10}
genus=${11}
species=${12}

output_medium=${13}
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

if [ "$caught_kept_preferred" = "preferred" ]
then
	species_most_frequently_preferred.sh	$application		\
						$process_name		\
						"$begin_year_month"	\
						"$end_year_month"	\
						"$fishing_purpose"	\
						"$caught_more_than"	\
						"$aggregate_level"	\
						"$by_area_yn"		\
						"$family"		\
						"$genus"		\
						"$species"		\
						"$output_medium"
	exit 0
fi

# Constants
# ---------
document_root=$(get_document_root.e)
row_file="/tmp/row_${process_name}_$$"
denominator_file="/tmp/denominator_${process_name}_$$"
output_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"

# Variables
# ---------
fishing_trips=`get_table_name $application fishing_trips`
catches=`get_table_name $application catches`
species_table=`get_table_name $application species`
fishing_trips=`get_table_name $application fishing_trips`

process_title=$(echo $fishing_purpose species_most_frequently $caught_kept_preferred From $begin_year_month To $end_year_month | format_initial_capital.e)

# Preprocess
# ----------
if [ "$caught_kept_preferred" = "caught" ]
then
	caught_kept_heading="Caught"
	sum_kept_released="sum( kept_count + released_count )"
elif [ "$caught_kept_preferred" = "kept" ]
then
	caught_kept_heading="Kept"
	sum_kept_released="sum( kept_count )"
else
	echo "Invalid caught_kept_preferred: $caught_kept_perferred" 1>&2
	exit 1
fi

if [ "$aggregate_level" = "annually" ]
then
	if [ "$by_area_yn" = "y" ]
	then
		first_select="substr( $catches.census_date, 1, 4 ),$fishing_trips.fishing_area"
		heading="Year,Area,Code,Common,Family,Genus,Species,$caught_kept_heading,Denominator,Percent"
	else
		first_select="substr( $catches.census_date, 1, 4 )"
		heading="Year,Code,Common,Family,Genus,Species,$caught_kept_heading,Denominator,Percent"
	fi

elif [ "$aggregate_level" = "year_month" ]
then
	if [ "$by_area_yn" = "y" ]
	then
		first_select="substr( $catches.census_date, 1, 7 ),$fishing_trips.fishing_area"
		heading="Year-Month,Area,Code,Common,Family,Genus,Species,$caught_kept_heading,Denominator,Percent"
	else
		first_select="substr( $catches.census_date, 1, 7 )"
		heading="Year-Month,Code,Common,Family,Genus,Species,$caught_kept_heading,Denominator,Percent"
	fi

elif [ "$aggregate_level" = "monthly" ]
then
	if [ "$by_area_yn" = "y" ]
	then
		first_select="substr( $catches.census_date, 6, 2 ),$fishing_trips.fishing_area"
		heading="Month,Area,Code,Common,Family,Genus,Species,$caught_kept_heading,Denominator,Percent"
	else
		first_select="substr( $catches.census_date, 6, 2 )"
		heading="Month,Code,Common,Family,Genus,Species,$caught_kept_heading,Denominator,Percent"
	fi
fi

if [ "$by_area_yn" = "y" ]
then
	justification="left,left,right,left,left,left,left,right,right,right"
else
	justification="left,right,left,left,left,left,right,right,right"
fi

if [ "$output_medium" = "text_file" ]
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

if [ "$family" != "" -a "$family" != "family" ]
then
	species_where=`or_sequence.e					   \
"${species_table}.family,${species_table}.genus,${species_table}.species"  \
		"${family},${genus},${species}"`
else
	species_where="1 = 1"
fi

# -------
# Process
# -------

# ----------------------
# Group by date and area
# ----------------------
if [ "$by_area_yn" = "y" ]
then
	fishing_trips_join="$catches.fishing_purpose = $fishing_trips.fishing_purpose and $catches.census_date = $fishing_trips.census_date and $catches.interview_location = $fishing_trips.interview_location and $catches.interview_number = $fishing_trips.interview_number"

	# Where numerator
	# ---------------
	if [ "$fishing_purpose" != "" ]
	then
	  where="$catches.fishing_purpose = '$fishing_purpose'
	       and $catches.family = $species_table.family
	       and $catches.genus = $species_table.genus
	       and $catches.species = $species_table.species
	       and ( kept_count > 0 or released_count > 0 )
	       and $fishing_trips_join
	       and fishing_area is not null
	       and $catches.census_date between   '${begin_year_month}-01' and
					          '${end_year_month}-31'
	       and $species_where"
	else
	  where="$catches.family = $species_table.family
	       and $catches.genus = $species_table.genus
	       and $catches.species = $species_table.species
	       and ( kept_count > 0 or released_count > 0 )
	       and $fishing_trips_join
	       and fishing_area is not null
	       and $catches.census_date between   '${begin_year_month}-01' and
					          '${end_year_month}-31'
	       and $species_where"
	fi

	# Select numerator
	# ----------------
	echo "select $first_select, florida_state_code,common_name,$catches.family, $catches.genus, $catches.species, $sum_kept_released caught
	from $catches, $species_table, $fishing_trips
	where $where
	group by $first_select,$catches.family,$catches.genus,$catches.species
	having caught >= $caught_more_than
	order by $first_select, caught desc;"			|
	sql.e ','						|
	sed 's/,/|/1'						|
	cat > $row_file

	# Where denominator
	# -----------------
	if [ "$fishing_purpose" != "" ]
	then
	   where="$catches.fishing_purpose = '$fishing_purpose'
	     and ( kept_count > 0 or released_count > 0 )
	     and $fishing_trips_join
	     and fishing_area is not null
	     and $catches.census_date between	'${begin_year_month}-01' and
						'${end_year_month}-31'"
	else
	   where="( kept_count > 0 or released_count > 0 )
	     and $fishing_trips_join
	     and fishing_area is not null
	     and $catches.census_date between	'${begin_year_month}-01' and
						'${end_year_month}-31'"
	fi

	# Select denominator
	# ------------------
	echo "select $first_select, $sum_kept_released caught
	from $catches,$fishing_trips
	where $where
	group by $first_select
	having caught >= $caught_more_than;"			|
	sql.e ','						|
	sed 's/,/|/1'						|
	cat > $denominator_file
	
	(
	echo "$title_maybe"
	echo "$heading_maybe"
	join -t',' -a1 -i $row_file $denominator_file		|
	sed 's/|/,/1'						|
	awk 'BEGIN {
		FS = ","
	}
	{
		year_month = $1
		fishing_area=$2
		florida_state_code = $3
		common_name = $4
		family = $5
		genus = $6
		species = $7
		caught_count = $8
		denominator = $9

		results = ( caught_count / denominator ) * 100
		printf ( "%s,%s,%s,%s,%s,%s,%s,%d,%d,%.3f\n",
			year_month,
			fishing_area,
			florida_state_code,
			common_name,
			family,
			genus,
			species,
			caught_count,
			denominator,
			results )
	}'
	) | cat > $output_file
else
# -------------
# Group by date
# -------------
	# Where numerator
	# ----------------
	if [ "$fishing_purpose" != "" ]
	then
	where="fishing_purpose = '$fishing_purpose'
	       and $catches.family = $species_table.family
	       and $catches.genus = $species_table.genus
	       and $catches.species = $species_table.species
	       and ( kept_count > 0 or released_count > 0 )
	       and $catches.census_date between	'${begin_year_month}-01' and
						'${end_year_month}-31'
	       and $species_where"
	else
	where="$catches.family = $species_table.family
	       and $catches.genus = $species_table.genus
	       and $catches.species = $species_table.species
	       and ( kept_count > 0 or released_count > 0 )
	       and $catches.census_date between	'${begin_year_month}-01' and
						'${end_year_month}-31'
	       and $species_where"
	fi

	# Select numerator
	# ----------------
	echo "select $first_select, florida_state_code,common_name,$catches.family, $catches.genus, $catches.species, $sum_kept_released caught
	from $catches, $species_table
	where $where
	group by $first_select,$catches.family,$catches.genus,$catches.species
	having caught >= $caught_more_than
	order by $first_select, caught desc;"			|
	sql.e ',' > $row_file

	# Where denominator
	# -----------------
	if [ "$fishing_purpose" != "" ]
	then
	     where="fishing_purpose = '$fishing_purpose'
	            and ( kept_count > 0 or released_count > 0 )
	            and $catches.census_date between '${begin_year_month}-01'
		    				 and '${end_year_month}-31'"
	else
	     where="( kept_count > 0 or released_count > 0 )
	            and $catches.census_date between '${begin_year_month}-01'
		    				 and '${end_year_month}-31'"
	fi

	# Select denominator
	# ------------------
	echo "select $first_select, $sum_kept_released caught
	from $catches
	where $where
	group by $first_select
	having caught >= $caught_more_than;"			|
	sql.e ',' > $denominator_file

	(
	echo "$title_maybe"
	echo "$heading_maybe"
	join -t',' -a1 -i $row_file $denominator_file		|
	awk 'BEGIN {
		FS = ","
	}
	{
		year_month = $1
		florida_state_code = $2
		common_name = $3
		family = $4
		genus = $5
		species = $6
		caught_count = $7
		denominator = $8
		results = ( caught_count / denominator ) * 100
		printf ( "%s,%s,%s,%s,%s,%s,%d,%d,%.3f\n",
			year_month,
			florida_state_code,
			common_name,
			family,
			genus,
			species,
			caught_count,
			denominator,
			results )
	}'
	) | cat > $output_file
fi

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$output_medium" = "text_file" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
elif [ "$output_medium" = "table" ]
then
	cat $output_file				|
	html_table.e "" "$heading" "," "$justification"
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

process_table_name=`get_table_name $application process`

echo "	update $process_table_name
	set execution_count =
		if(execution_count,execution_count+1,1)
	where process = '$process_name';"		|
sql.e

rm -f $row_file
rm -f $denominator_file

exit 0

