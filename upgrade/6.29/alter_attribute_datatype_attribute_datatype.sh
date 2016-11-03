#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

(
table_name=`get_table_name $application attribute_datatype`
echo "alter table $table_name modify attribute_datatype char (20) not null;" | sql.e
attribute_table=`get_table_name $application attribute`
echo "delete from $attribute_table where attribute = 'attribute_datatype';" | sql.e
echo "insert into $attribute_table					\
	(attribute,							\
	hint_message,							\
	appaserver_yn,							\
	attribute_datatype,						\
	width,								\
	float_decimal_places,						\
	post_change_javascript,						\
	on_focus_javascript_function,					\
	lookup_histogram_output_yn,					\
	lookup_time_chart_output_yn)					\
	values('attribute_datatype','','y','text',20,0,'','','n','n');"	 |
sql.e									  
) 2>&1 | grep -vi duplicate
exit 0
