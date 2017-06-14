:
# $APPASERVER_HOME/src_creel/additional_species_select.sh
# -------------------------------------------------------

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

exclude="${family}^${genus}^${species}"

select="family,genus,species"
primary_key_list="fishing_purpose,census_date,interview_location,interview_number"

fishing_trips_for_species.sh	creel				\
				$begin_date			\
				$end_date			\
				$fishing_purpose		\
				$family				\
				$genus				\
				$species			|
where.e $primary_key_list '^'					|
sed "s/.*/select $select from catches where &;/"		|
sql.e								|
grep -v "$exclude"						|
sort								|
group.e								|
sort -rn							|
cat

exit 0
