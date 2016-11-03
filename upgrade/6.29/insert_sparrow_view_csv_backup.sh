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
role_process=`get_table_name $application role_process`
role_process_set_member=`get_table_name $application role_process_set_member`
role_operation=`get_table_name $application role_operation`
(
cat << all_done
delete from $process where process = 'view_CSV_backup';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display) values ('view_CSV_backup','view_csv_backup \$application \$process \$login_name sparrowbackup','View the nightly backup formatted to load into a spreadsheet.',null,null,null,'output',null);
insert into $role_process (role,process) values ('supervisor','view_CSV_backup');
delete from $process_parameter where process = 'view_CSV_backup';
delete from $javascript_processes where process = 'view_CSV_backup';
delete from $process_generic_output where process = 'view_CSV_backup';
insert into $process_groups (process_group) values ('output');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
