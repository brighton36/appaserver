:
# $APPASERVER_HOME/src_creel/additional_species_select_detail.sh
# --------------------------------------------------------------

if [ "$#" -ne 6 ]
then
	echo "Usage: $0 fishing_purpose begin_date end_date family genus species" 1>&2
	exit 1
fi

fishing_purpose=$1
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	fishing_purpose="sport"		# Defaults to sport
fi

begin_date=$2
if [ "$begin_date" = "" -o "$begin_date" = "begin_date" ]
then
	begin_date="1901-01-01"
fi

end_date=$3
if [ "$end_date" = "" -o "$end_date" = "end_date" ]
then
	end_date=`now.sh ymd`
fi

family=$4
genus=$5
species=$6

select="catches.census_date,catches.interview_location,catches.interview_number,interview_time,species_preferred,fishing_area,dayname( catches.census_date ),researcher,fishing_party_composition,recreational_angler_reside,trip_hours,number_of_people_fishing,hours_fishing,catches.family,catches.genus,catches.species,kept_count,released_count"

from="catches,fishing_trips,creel_census"

primary_key_list="catches.fishing_purpose,catches.census_date,catches.interview_location,catches.interview_number"

join_where=" and catches.fishing_purpose = fishing_trips.fishing_purpose and catches.census_date = fishing_trips.census_date and catches.interview_location = fishing_trips.interview_location and catches.interview_number = fishing_trips.interview_number and fishing_trips.fishing_purpose = creel_census.fishing_purpose and fishing_trips.census_date = creel_census.census_date and fishing_trips.interview_location = creel_census.interview_location"

fishing_trips_for_species.sh	creel					\
				$begin_date				\
				$end_date				\
				$fishing_purpose			\
				$family					\
				$genus					\
				$species				|
where.e $primary_key_list '^'						|
sed "s/$/$join_where/"							|
sed "s/.*/select $select from $from where &;/"				|
sql.e									|
sort									|
cat

exit 0
