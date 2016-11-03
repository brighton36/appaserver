:
# src_hydrology/delete_alternative.sh
# -----------------------------------

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 model alternative [station]" 1>&2
	exit 1
fi

model=$1
alternative=$2

if [ "$#" -eq 3 ]
then
	select_station=$3
fi

alternative_field_name=alt
delete_field_list=model,alt,station,datatype
select_list=station,datatype
alternative_station_datatype_table=alt_station_datatype
alternative_table=alt
#delete_field_list=model,alternative,station,datatype
#alternative_field_name=alternative
#alternative_station_datatype_table=alternative_station_datatype
#alternative_table=alternative

for i in			 				 \
	`echo "select $select_list	 			 \
	 from $alternative_station_datatype_table		 \
	 where model = '$model'					 \
	   and $alternative_field_name = '${alternative}';"	|\
	 sql.e '^'`
do
	station=`echo $i | piece.e '^' 0`

	if [ "$#" -eq 3 -a "$station" != "$select_station" ]
	then
		continue
	fi

	datatype=`echo $i | piece.e '^' 1`

echo "Deleting model/alternative/station/datatype = ${model}/${alternative}/${station}/${datatype}"

	# Delete from model_results
	# -------------------------
	echo "${model}|${alternative}|${station}|${datatype}"	|
	delete_statement.e					\
		table_name=model_results		 	\
		field_list=$delete_field_list			|
	sql.e							|
	cat

	# Delete from alternative_station_datatype
	# ----------------------------------------
	echo "${model}|${alternative}|${station}|${datatype}"	|
	delete_statement.e					\
		table_name=$alternative_station_datatype_table	\
		field_list=$delete_field_list			|
	sql.e							|
	cat

done

if [ "$#" -eq 2 ]
then
# 	Delete from alternative
# 	-----------------------
	echo "${model}|${alternative}"				|
	delete_statement.e					\
		table_name=$alternative_table			\
		field_list=$delete_field_list			|
	sql.e							|
	cat
fi

exit 0

