#!/bin/bash
if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "$ . set_database" 1>&2
	exit 1
fi

if [ "$application" != "hydrology" -a "$application" != "audubon" ]
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
delete from $process where process = 'output_merged_datasets';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_merged_datasets','merged_datasets_output \$person \$session \$application \$role aggregate_level \$dictionary',null,'process_merged_help.html',null,null,'output',null,'post_change_merged_datasets(this)');
delete from $process_parameter where process = 'output_merged_datasets';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','null','null','begin_date','null','3','n',null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','null','null','display_count_yn','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','null','null','end_date','null','4','n',null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','null','null','null','aggregate_level','5','n',null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','null','null','null','merged_output','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','null','null','null','validation_level','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','null','null','omit_output_if_any_missing_yn','null','8',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','station','null','null','null','1','y','y',null,'station_map');
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_merged_datasets','station_datatype','null','null','null','2','y',null,null,null);
delete from $javascript_processes where process = 'output_merged_datasets';
insert into $javascript_processes (javascript_filename,process) values ('merged_datasets_process.js','output_merged_datasets');
delete from $process_generic_output where process = 'output_merged_datasets';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('display_count_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('omit_output_if_any_missing_yn',null,null,null,'1');
insert into $process_groups (process_group) values ('output');
insert into $javascript_files (javascript_filename) values ('merged_datasets_process.js');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_level','daily, monthly, etc...',null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','annually','9');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','daily','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','half_hour','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','hourly','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','monthly','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','real_time','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','weekly','5');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('merged_output',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merged_output','googlechart','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merged_output','gracechart','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merged_output','spreadsheet','5');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merged_output','table','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('merged_output','text_file','3');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('validation_level',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('validation_level','either','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('validation_level','provisional','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('validation_level','validated','2');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'station_map';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('station_map','google_map_station_process \$application \$login_name',null,null,null,null,null,null,null);
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
