#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

if [ "$application" != timriley ]
then
	exit 0
fi

export DATABASE=$application

(
table_name=`get_table_name $application bank_upload`
echo "alter table $table_name add bank_upload_date_time datetime;" | sql.e
folder_attribute_table=`get_table_name $application folder_attribute`
echo "insert into $folder_attribute_table				\
	(folder,attribute,primary_key_index,display_order,		\
	 omit_insert_yn,omit_insert_prompt_yn,omit_update_yn)		\
	values								\
	('bank_upload','bank_upload_date_time',0,4,'n','n','n');" 				 |
sql.e
attribute_table=`get_table_name $application attribute`
echo "insert into $attribute_table					\
	(attribute,attribute_datatype,width,float_decimal_places,	\
	 hint_message,appaserver_yn)					\
	values								\
	('bank_upload_date_time','current_date_time',19,0,'','n');"					 |
sql.e
) 2>&1 | grep -vi duplicate
exit 0
