#!/bin/bash
# src_capitolpops/membership_position_list.sh
# -------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application process_name output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
fi

process_name=$2

output_medium=$3
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

# Variables
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"
process_title=`echo "$process_name" | format_initial_capital.e`
musician_file=/tmp/musician_file_$$.dat
position_file=/tmp/position_file_$$.dat
heading="Instrument,Full Name,Positions"
justification="left,left,left"

# Process
# -------
echo "	select musician.primary_instrument, musician.full_name
	from musician, band_member, instrument, section
	where musician.full_name = band_member.full_name
	  and musician.primary_instrument = instrument.instrument
	  and instrument.section = section.section
	  and exists (	select 1
			from member_position
			where	member_position.full_name =
				band_member.full_name
			  and	position != 'student' )
	order by	instrument.sort_order,
			instrument.instrument,
			musician.last_name;"				|
sql.e									|
cat > $musician_file

echo "	select	musician.primary_instrument,
		musician.full_name,
		position
	from	musician,
		band_member,
		instrument,
		section,
		member_position
	where musician.full_name = band_member.full_name
	  and band_member.full_name = member_position.full_name
	  and musician.primary_instrument = instrument.instrument
	  and instrument.section = section.section
	  and member_position.position != 'student'
	order by	instrument.sort_order,
			instrument.instrument,
			musician.last_name,
			position;"					|
sql.e '|'								|
sed 's/|/^/1'								|
cat > $position_file


# Output
# ------
echo "$heading" > $output_file
join.e $musician_file $position_file '|' ';'				|
tr '|' ','								|
tr '^' ','								|
cat >> $output_file


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
	grep -v "$heading"				|
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

rm $musician_file $position_file
exit 0
