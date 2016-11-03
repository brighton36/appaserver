:
# ---------------------------------------------
# related_folder_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo \
"Usage: $0 application delimiter" 1>&2
	exit 1
fi

application=$1
delimiter=$2

relation_table=`get_table_name $application relation`

order="pair_1tom_order,folder,related_folder,related_attribute"

echo "select 		folder,						\
			related_folder,					\
			related_attribute, 				\
			pair_1tom_order,				\
			omit_1tom_detail_yn,				\
			prompt_mto1_recursive_yn,			\
			relation_type_isa_yn,				\
			copy_common_attributes_yn,			\
			automatic_preselection_yn,			\
			drop_down_multi_select_yn,			\
			join_1tom_each_row_yn,				\
			omit_lookup_before_drop_down_yn,		\
			hint_message					\
	      from $relation_table					\
	      order by $order;"						|
sql.e "$delimiter"

