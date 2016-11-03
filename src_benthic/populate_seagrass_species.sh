:
# src_benthic/populate_seagrass_species.sh
# ----------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application one2m_folder_name_for_processes" 1>&2
	exit 1
fi

echo "$0 $*" 1>&2

application=$1
one2m_folder_name=$2

# See MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER
# ----------------------------------------
select="vegetation_name,'|',vegetation_species_code,'-',vegetation_name"

if [ "$one2m_folder_name" = "quad_vegetation" ]
then
	where="vegetation_name not like '%total%' and vegetation_species_code is not null"
	order="vegetation_species_code,vegetation_name"
elif [ "$one2m_folder_name" = "quad_vegetation_total" ]
then
	where="vegetation_name like '%total%' and vegetation_species_code is not null"
	order="display_order"
else
	select="vegetation_name,'|',vegetation_name,'-',vegetation_species_code"
	where="1 = 1"
	order="vegetation_name"
fi

table=$(get_table_name $application vegetation_species)

echo "select $select from $table where $where order by $order;" 	|
sql.e									|
sed 's/\^//g'								|
cat

exit 0
