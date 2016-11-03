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

process=`get_table_name $application process`
operation=`get_table_name $application operation`
process_parameter=`get_table_name $application process_parameter`
javascript_processes=`get_table_name $application javascript_processes`
process_generic_output=`get_table_name $application process_generic_output`
javascript_files=`get_table_name $application javascript_files`
prompt=`get_table_name $application prompt`
process_groups=`get_table_name $application process_groups`
drop_down_prompt=`get_table_name $application drop_down_prompt`
drop_down_prompt_data=`get_table_name $application drop_down_prompt_data`
(
cat << all_done
delete from $process where process = 'update_inside_seven_minute_count';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_group,appaserver_yn,process_set_display) values ('update_inside_seven_minute_count','update_inside_seven_minute_count.sh \$application \$state quad_sheet site_number visit_date',null,null,null,null,null,null);
delete from $process_parameter where process = 'update_inside_seven_minute_count';
delete from $javascript_processes where process = 'update_inside_seven_minute_count';
delete from $process_generic_output where process = 'update_inside_seven_minute_count';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
