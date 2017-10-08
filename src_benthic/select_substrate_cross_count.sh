:
echo "$0 $*" 1>&2

if [ "$#" != 7 ]
then
	echo "Usage: $0 application begin_date end_date substrate family genus species" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3
substrate=$4
family_name=$5
genus_name=$6
species_name=$7

(
select_substrate_cross_count_exists.sh $application $begin_date $end_date $substrate "$family_name" "$genus_name" "$species_name"
select_substrate_cross_count_not_exists.sh $application $begin_date $end_date $substrate "$family_name" "$genus_name" "$species_name"
) | sort --field-separator='^' --key=4 -n

exit 0

