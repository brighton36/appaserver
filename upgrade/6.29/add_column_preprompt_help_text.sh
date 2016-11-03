#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

(
table_name=`get_table_name $application process`
echo "alter table $table_name add preprompt_help_text text;" | sql.e
folder_attribute_table=`get_table_name $application folder_attribute`
echo "insert into $folder_attribute_table				\
	(folder,attribute,primary_key_index,display_order,		\
	 omit_insert_yn,omit_insert_prompt_yn)				\
	values								\
	('process','preprompt_help_text',0,8,'n','n');"	 				 |
sql.e
attribute_table=`get_table_name $application attribute`
echo "insert into $attribute_table					\
	(attribute,attribute_datatype,width,float_decimal_places,	\
	 hint_message,appaserver_yn)					\
	values								\
	('preprompt_help_text','notepad',1024,0,'','y');"					 |
sql.e
) 2>&1 | grep -vi duplicate
exit 0
