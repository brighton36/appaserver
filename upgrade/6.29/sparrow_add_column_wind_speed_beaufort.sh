#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != sparrow ]
then
	exit 0
fi

(
table_name=`get_table_name $application site_visit`
echo "alter table $table_name add wind_speed_beaufort integer;" | sql.e
folder_attribute_table=`get_table_name $application folder_attribute`
echo "insert into $folder_attribute_table				\
	(folder,attribute,primary_key_index,display_order,		\
	 omit_insert_yn,omit_insert_prompt_yn)				\
	values								\
	('site_visit','wind_speed_beaufort',0,7,'n','n');"	 				 |
sql.e
attribute_table=`get_table_name $application attribute`
echo "insert into $attribute_table					\
	(attribute,attribute_datatype,width,float_decimal_places,	\
	 hint_message,appaserver_yn)					\
	values								\
	('wind_speed_beaufort','integer',1,0,'Scale from 1 to 4.','n');"					 |
sql.e
) 2>&1 | grep -vi duplicate
exit 0
