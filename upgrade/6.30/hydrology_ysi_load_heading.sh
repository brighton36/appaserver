#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != audubon -a "$application" != hydrology ]
then
	exit 0
fi

(
table_name=`get_table_name $application datatype`
echo "alter table $table_name modify ysi_load_heading char (25);" | sql.e
attribute_table=`get_table_name $application attribute`
echo "delete from $attribute_table where attribute = 'ysi_load_heading';" | sql.e
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
	values('ysi_load_heading','','n','text',25,0,'','','n','n');"	 |
sql.e									  
) 2>&1 | grep -vi duplicate
exit 0
