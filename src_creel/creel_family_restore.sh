:
# $APPASERVER_HOME/src_creel/creel_family_restore.sh
# -------------------------------------------------------
# Execute this if you inadvertently deleted a family
# and need to restore the records from a backup database.
# -------------------------------------------------------

input=creel_family_deleted.dat

family_output=creel_families.sql
genera_output=creel_genera.sql
species_output=creel_species.sql
catches_output=creel_catches.sql
catch_measurements_output=creel_catch_measurements.sql

function export_families()
{
	family=$1

	select="family,orders,common_name"
	where="family = '$family'"
	table=families

	echo "select $select from $table where $where;"		|
	sql '|'							|
	insert_statement.e table=$table field=$select		|
	cat >> $family_output
}

function export_genera()
{
	family=$1

	select="family,genus,common_name"
	where="family = '$family'"
	table=genera

	echo "select $select from $table where $where;"		|
	sql '|'							|
	insert_statement.e table=$table field=$select		|
	cat >> $genera_output
}

function export_species()
{
	family=$1

	select="genus,species,common_name,taxonomic_serial_number,family,measurement_termination,florida_state_code"

	where="family = '$family'"
	table=species

	echo "select $select from $table where $where;"		|
	sql '|'							|
	insert_statement.e table=$table field=$select		|
	cat >> $species_output
}

function export_catches()
{
	family=$1

	select="census_date,interview_location,interview_number,genus,species,kept_count,released_count,family,fishing_purpose,last_person_validating"

	where="family = '$family'"
	table=catches

	echo "select $select from $table where $where;"		|
	sql '|'							|
	insert_statement.e table=$table field=$select		|
	cat >> $catches_output
}

function export_catch_measurements()
{
	family=$1

	select="census_date,interview_location,interview_number,genus,species,catch_measurement_number,length_millimeters,family,fishing_purpose,last_person_validating"

	where="family = '$family'"
	table=catch_measurements

	echo "select $select from $table where $where;"		|
	sql '|'							|
	insert_statement.e table=$table field=$select		|
	cat >> $catch_measurements_output
}

rm $family_output 2>/dev/null
rm $genera_output 2>/dev/null
rm $species_output 2>/dev/null
rm $catches_output 2>/dev/null
rm $catch_measurements_output 2>/dev/null

cat $input							|
while read family
do
	echo $family
	export_families $family
	export_genera $family
	export_species $family
	export_catches $family
	export_catch_measurements $family
done

exit 0

