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
delete from $process where process = 'create_empty_application';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_group,process_set_display,appaserver_yn) values ('create_empty_application','create_empty_application \$application \$session \$login_name \$role \$process destination_application new_application_title y delete_application_yn execute_yn','This process creates an empty application. It creates a new database, the Appaserver tables, the data directories, among other application objects. Following this process, you can begin inserting the folder rows.',null,null,null,null,'y');
delete from $process_parameter where process = 'create_empty_application';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('create_empty_application','null','null','delete_application_yn','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('create_empty_application','null','null','destination_application','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('create_empty_application','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('create_empty_application','null','null','new_application_title','null','2',null,null,null,null);
delete from $javascript_processes where process = 'create_empty_application';
delete from $process_generic_output where process = 'create_empty_application';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('delete_application_yn','Delete the existing application?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('destination_application',null,null,null,'30');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('new_application_title',null,null,null,'30');
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
