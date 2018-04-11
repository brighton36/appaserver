#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != waterquality ]
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
delete from $process where process = 'load_FIU_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('load_FIU_file','load_concentration_file \$application \$process project_name filename station execute_yn',null,null,null,null,'load',null,null);
insert into $role_process (role,process) values ('scientist','load_FIU_file');
insert into $role_process (role,process) values ('supervisor','load_FIU_file');
delete from $process_parameter where process = 'load_FIU_file';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_FIU_file','null','null','execute_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_FIU_file','null','null','filename','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_FIU_file','water_project','null','null','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_FIU_file';
delete from $process_generic_output where process = 'load_FIU_file';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
