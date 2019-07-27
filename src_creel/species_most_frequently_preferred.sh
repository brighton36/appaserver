:
# src_creel/species_most_frequently_preferred.sh
# ----------------------------------------------

# Input
# -----
echo "$0" "$*" 1>&2

if [ "$#" -ne 12 ]
then
	echo "Usage: $0 application process_name begin_year_month end_year_month fishing_purpose caught_more_than aggregate_level by_area_yn family genus species output_medium" 1>&2
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

family=$9
genus=${10}
species=${11}

output_medium=${12}
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
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
species_table=`get_table_name $application species`

process_title=$(echo $fishing_purpose species_most_frequently_preferred From $begin_year_month To $end_year_month | format_initial_capital.e)

# Preprocess
# ----------
if [ "$aggregate_level" = "annually" ]
then
	if [ "$by_area_yn" = "y" ]
	then
		first_select="substr( $fishing_trips.census_date, 1, 4 ),$fishing_trips.fishing_area"
		heading="Year,Area,Code,Common,Family,Genus,Species Preferred,Count,Denominator,Percent"
		justification="left,left,right,left,left,left,left,right,right,right"
	else
		first_select="substr( census_date, 1, 4 )"
		heading="Year,Code,Common,Family,Genus,Species Preferred,Count,Denominator,Percent"
		justification="left,right,left,left,left,left,right,right,right"
	fi
elif [ "$aggregate_level" = "year_month" ]
then
	if [ "$by_area_yn" = "y" ]
	then
		first_select="substr( census_date, 1, 7 ),$fishing_trips.fishing_area"
		heading="Year-Month,Area,Code,Common,Family,Genus,Species Preferred,Count,Denominator,Percent"
		justification="left,left,right,left,left,left,left,right,right,right"
	else
		first_select="substr( census_date, 1, 7 )"
		heading="Year-Month,Code,Common,Family,Genus,Species Preferred,Count,Denominator,Percent"
		justification="left,right,left,left,left,left,right,right,right"
	fi
elif [ "$aggregate_level" = "monthly" ]
then
	if [ "$by_area_yn" = "y" ]
	then
		first_select="substr( census_date, 6, 2 ),$fishing_trips.fishing_area"
		heading="Month,Area,Code,Common,Family,Genus,Species Preferred,Count,Denominator,Percent"
		justification="left,left,right,left,left,left,left,right,right,right"
	else
		first_select="substr( census_date, 6, 2 )"
		heading="Month,Code,Common,Family,Genus,Species Preferred,Count,Denominator,Percent"
		justification="left,right,left,left,left,left,right,right,right"
	fi
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
	# Where numerator
	# ---------------
	if [ "$fishing_purpose" != "$fishing_purpose" ]
	then
		where="fishing_purpose = '$fishing_purpose'
	  	and $fishing_trips.family = $species_table.family
	  	and $fishing_trips.genus = $species_table.genus
	  	and $fishing_trips.species_preferred = $species_table.species
	  	and species_preferred is not null
	  	and $fishing_trips.census_date between
			'${begin_year_month}-01' and '${end_year_month}-31'
	  	and fishing_area is not null
	  	and $species_where"
	else
		where="$fishing_trips.family = $species_table.family
	  	and $fishing_trips.genus = $species_table.genus
	  	and $fishing_trips.species_preferred = $species_table.species
	  	and species_preferred is not null
	  	and $fishing_trips.census_date between
			'${begin_year_month}-01' and '${end_year_month}-31'
	  	and fishing_area is not null
	  	and $species_where"
	fi

	# Select numerator
	# ----------------
	echo "select $first_select, florida_state_code,common_name,$fishing_trips.family, $fishing_trips.genus, $fishing_trips.species_preferred, count(*)
	from $fishing_trips, $species_table
	where $where
	group by	$first_select,
			$fishing_trips.family,
			$fishing_trips.genus,
			$fishing_trips.species_preferred
	having count(*) >= $caught_more_than
	order by $first_select, count(*) desc;"			|
	sql.e ','						|
	sed 's/,/|/1'						|
	cat > $row_file

	# Where denominator
	# -----------------
	if [ "$fishing_purpose" != "" ]
	then
		where="fishing_purpose = '$fishing_purpose'
	  		and species_preferred is not null
	  		and $fishing_trips.census_date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
	  		and fishing_area is not null"
	else
		where="species_preferred is not null
	  		and $fishing_trips.census_date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
	  		and fishing_area is not null"
	fi

	# Select denominator
	# ------------------
	echo "select $first_select, count(*)
	from $fishing_trips
	where $where
	group by $first_select
	having count(*) >= $caught_more_than;"			|
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
		area = $2
		florida_state_code = $3
		common_name = $4
		family = $5
		genus = $6
		species = $7
		preferred_count = $8
		denominator = $9
		results = ( preferred_count / denominator ) * 100
		printf ( "%s,%s,%s,%s,%s,%s,%s,%d,%d,%.3f\n",
			year_month,
			area,
			florida_state_code,
			common_name,
			family,
			genus,
			species,
			preferred_count,
			denominator,
			results )
	}'
	) | cat > $output_file
else
	# Where numerator
	# ---------------
	if [ "$fishing_purpose" != "" ]
	then
		where="fishing_purpose = '$fishing_purpose'
	  		and $fishing_trips.family = $species_table.family
	  		and $fishing_trips.genus = $species_table.genus
	  		and $fishing_trips.species_preferred =
				$species_table.species
	  		and species_preferred is not null
	  		and $fishing_trips.census_date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
	  		and $species_where"
	else
		where="$fishing_trips.family = $species_table.family
	  		and $fishing_trips.genus = $species_table.genus
	  		and $fishing_trips.species_preferred =
				$species_table.species
	  		and species_preferred is not null
	  		and $fishing_trips.census_date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
	  		and $species_where"
	fi

	# Select numerator
	# ----------------
	echo "select $first_select, florida_state_code,common_name,$fishing_trips.family, $fishing_trips.genus, $fishing_trips.species_preferred, count(*)
	from $fishing_trips, $species_table
	where $where
	group by	$first_select,
			$fishing_trips.family,
			$fishing_trips.genus,
			$fishing_trips.species_preferred
	having count(*) >= $caught_more_than
	order by $first_select, count(*) desc;"			|
	sql.e ',' > $row_file
	
	# Where denominator
	# -----------------
	if [ "$fishing_purpose" != "" ]
	then
		where="fishing_purpose = '$fishing_purpose'
	  		and species_preferred is not null
	  		and $fishing_trips.census_date between
					'${begin_year_month}-01' and
					'${end_year_month}-31'"
	else
		where="species_preferred is not null
	  		and $fishing_trips.census_date between
					'${begin_year_month}-01' and
					'${end_year_month}-31'"
	fi

	# Select denominator
	# ------------------
	echo "select $first_select, count(*)
	from $fishing_trips
	where $where
	group by $first_select
	having count(*) >= $caught_more_than;"			|
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
		preferred_count = $7
		denominator = $8
		results = ( preferred_count / denominator ) * 100
		printf ( "%s,%s,%s,%s,%s,%s,%d,%d,%.3f\n",
			year_month,
			florida_state_code,
			common_name,
			family,
			genus,
			species,
			preferred_count,
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

rm -f $row_file
rm -f $denominator_file

exit 0

