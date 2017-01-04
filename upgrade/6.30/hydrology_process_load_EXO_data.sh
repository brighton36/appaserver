#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != hydrology -a "$application" != audubon ]
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
delete from $process where process = 'load_EXO_data';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,process_group,appaserver_yn,preprompt_help_text,post_change_javascript) values ('load_EXO_data','load_ysi_data \$application \$process filename station begin_date begin_time end_date end_time change_existing_data_yn really_yn',null,null,null,'load',null,null,null);
insert into $role_process (role,process) values ('hydrologist','load_EXO_data');
insert into $role_process (role,process) values ('supervisor','load_EXO_data');
insert into $role_process (role,process) values ('technician','load_EXO_data');
delete from $process_parameter where process = 'load_EXO_data';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','begin_date','null','30',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','begin_time','null','35',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','change_existing_data_yn','null','60',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','end_date','null','40',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','end_time','null','45',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','execute_yn','null','90',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','filename','null','10',null,null,null,null);
delete from $javascript_processes where process = 'load_EXO_data';
delete from $process_generic_output where process = 'load_EXO_data';
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('begin_date',null,'Format:  YYYY-MM-DD','y','10');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('begin_time',null,'Format HHMM',null,'4');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('end_date',null,'Format:  YYYY-MM-DD','y','10');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('end_time',null,'Format HHMM',null,'4');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('execute_yn',null,'Do you really want to execute this PROCESS?',null,'1');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('filename','y',null,null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
