#!/bin/bash
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
	echo "$ . set_database" 1>&2
	exit 1
fi

if [ "$application" != "timriley" -a	\
     "$application" != "appahost" -a	\
     "$application" != "eves" -a	\
     "$application" != "capitolpops" -a	\
     "$application" != "rudek" ]
then
	exit 0
fi

(
table_name=`get_table_name $application bank_upload_event`
echo "alter table $table_name add feeder_account char (40);" | sql.e
folder_attribute_table=`get_table_name $application folder_attribute`
echo "insert into $folder_attribute_table				\
	(folder,attribute,primary_key_index,display_order,		\
	 omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,		\
	 additional_unique_index_yn,					\
	 additional_index_yn,						\
	 insert_required_yn,						\
	 lookup_required_yn )						\
	values								\
	('bank_upload_event','feeder_account',0,5,'n','n','n','n','n','n','n');" 	 |
sql.e
attribute_table=`get_table_name $application attribute`
echo "insert into $attribute_table					\
	(attribute,attribute_datatype,width,float_decimal_places,	\
	 hint_message,appaserver_yn,					\
	 post_change_javascript,					\
	 on_focus_javascript_function,					\
	 lookup_histogram_output_yn,					\
	 lookup_time_chart_output_yn )					\
	values								\
	('feeder_account','text',40,0,'External accounts like credit card and Pay Pal that feed in transactions.','n','','','n','n');"		 |
sql.e
relation_table=`get_table_name $application relation`
echo "insert into $relation_table					\
	(folder,related_folder,related_attribute)			\
	values								\
	('bank_upload_event','feeder_account','null');"						 |
sql.e
) 2>&1 | grep -vi duplicate
exit 0
