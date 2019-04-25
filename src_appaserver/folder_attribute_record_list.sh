#!/bin/sh
# -------------------------------------------------
# src_appaserver/folder_attribute_record_list.sh
# -------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 folder [delimiter]" 1>&2
	exit 1
fi

folder=$1

if [ "$#" -eq 2 ]
then
	delimiter=$2
else
	delimiter="^"
fi

attribute=`get_table_name $application attribute`
folder_attribute=`get_table_name $application folder_attribute`

order_clause=display_order,primary_key_index

echo "select 	$attribute.attribute,					\
		attribute_datatype,					\
		width, 							\
		float_decimal_places,					\
		primary_key_index,					\
		display_order,						\
		omit_insert_yn,						\
		omit_insert_prompt_yn,					\
		omit_update_yn,						\
		hint_message,						\
		$attribute.post_change_javascript,			\
		$attribute.on_focus_javascript_function,		\
		$folder_attribute.additional_unique_index_yn,		\
		$folder_attribute.additional_index_yn,			\
		$folder_attribute.lookup_required_yn,			\
		$folder_attribute.insert_required_yn,			\
		$attribute.lookup_histogram_output_yn,			\
		$attribute.lookup_time_chart_output_yn,			\
		$attribute.appaserver_yn				\
	      from	$attribute, 					\
			$folder_attribute 				\
	      where $folder_attribute.attribute = $attribute.attribute	\
		and folder = '$folder'					\
	      order by ${order_clause};"				|
sql.e "$delimiter"

exit 0
