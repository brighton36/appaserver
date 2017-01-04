#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

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
delete from $process where process = 'clone_folder';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,process_group,appaserver_yn,preprompt_help_text,post_change_javascript) values ('clone_folder','clone_folder \$application n \$session \$login_name \$role destination_application folder attribute old_data new_data html delete_yn execute_yn database_management_system output2file_yn',null,null,null,null,'y',null,null);
delete from $process_parameter where process = 'clone_folder';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','application','null','destination_application','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','attribute','null','null','null','1',null,'y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','folder_attribute','null','null','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','null','null','delete_yn','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','null','null','new_data','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','null','null','null','database_management_system','8',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','null','null','old_data','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','null','null','output2file_yn','null','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('clone_folder','null','null','execute_yn','null','9',null,null,null,null);
delete from $javascript_processes where process = 'clone_folder';
insert into $javascript_processes (javascript_filename,process) values ('clone_folder.js','clone_folder');
delete from $process_generic_output where process = 'clone_folder';
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('delete_yn',null,null,null,'1');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('destination_application',null,null,null,'20');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('new_data',null,null,null,'50');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('old_data',null,null,null,'50');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('output2file_yn',null,'preselected filename in /opt/html/everglades',null,'1');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $javascript_files (javascript_filename) values ('clone_folder.js');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('database_management_system','Defaults to mysql',null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('database_management_system','mysql','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('database_management_system','oracle','2');
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
