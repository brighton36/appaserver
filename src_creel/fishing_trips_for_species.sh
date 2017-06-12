:
# $APPASERVER_HOME/src_creel/fishing_trips_for_species.sh
# -------------------------------------------------------

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 application begin_date end_date fishing_purpose family genus species" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database

begin_date=$2
if [ "$begin_date" = "" -o "$begin_date" = "begin_date" ]
then
	begin_date="1901-01-01"
fi

end_date=$3
if [ "$end_date" = "" -o "$end_date" = "end_date" ]
then
	end_date=`date.e 0 | piece.e ':' 0`
fi

fishing_purpose=$4
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	fishing_purpose="sport"
fi

family=$5
genus=$6
species=$7

#family="Sciaenidae"
#genus="Cynoscion"
#species="nebulosus"

#fishing_purpose="sport"
#begin="2015-01-01"
#end="2015-01-31"

select="fishing_purpose,census_date,interview_location,interview_number"
folder="catches"
where="fishing_purpose = '$fishing_purpose' and census_date between '$begin_date' and '$end_date' and family = '$family' and genus = '$genus' and species = '$species'"

echo "select $select from $folder where $where;"	|
sql.e							|
sort

exit 0
