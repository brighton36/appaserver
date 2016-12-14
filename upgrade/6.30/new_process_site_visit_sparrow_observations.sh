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
delete from $process where process = 'site_visit_sparrow_observations';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_group,process_set_display,appaserver_yn,preprompt_help_text) values ('site_visit_sparrow_observations','site_visit_sparrow_observations.sh \$application \$process survey_year output_medium',null,null,null,'output',null,null,null);
insert into $role_process (role,process) values ('public','site_visit_sparrow_observations');
insert into $role_process (role,process) values ('scientist','site_visit_sparrow_observations');
insert into $role_process (role,process) values ('supervisor','site_visit_sparrow_observations');
delete from $process_parameter where process = 'site_visit_sparrow_observations';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('site_visit_sparrow_observations','null','null','null','site_visit_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('site_visit_sparrow_observations','null','null','survey_year','null','1',null,null,null,null);
delete from $javascript_processes where process = 'site_visit_sparrow_observations';
delete from $process_generic_output where process = 'site_visit_sparrow_observations';
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('survey_year',null,null,null,'4');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('site_visit_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('site_visit_output_medium','spreadsheet','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('site_visit_output_medium','table','1');
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
