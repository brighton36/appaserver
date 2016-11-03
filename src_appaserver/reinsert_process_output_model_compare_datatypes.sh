#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != everglades ]
then
	exit 0
fi

process=`get_table_name $application process`
operation=`get_table_name $application operation`
folder_operation=`get_table_name $application folder_operation`
process_parameter=`get_table_name $application process_parameter`
javascript_processes=`get_table_name $application javascript_processes`
javascript_files=`get_table_name $application javascript_files`
prompt=`get_table_name $application prompt`
drop_down_prompt=`get_table_name $application drop_down_prompt`
drop_down_prompt_data=`get_table_name $application drop_down_prompt_data`
(
cat << all_done
delete from $process where process = 'output_model_compare_datatypes';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn) values ('output_model_compare_datatypes','generic_overlay_output \$application \$role \$process_set \$dictionary',null,'process_model_compare_datatypes_help.html','post_change_compare_datatype(this)',null,null);
delete from $process_parameter where process = 'output_model_compare_datatypes';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','alternative','null','null','null','2','y','y',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','alternative_station_datatype','null','compare_station|compare_datatype','null','2','y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','alternative_station_datatype','null','null','null','1',null,'n',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','datatype','null','null','null','2','y','y',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','null','null','begin_date','null','6',null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','null','null','end_date','null','7',null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','null','null','null','compare_output','9',null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_drop_down_yn,populate_drop_down_process) values ('output_model_compare_datatypes','null','null','null','model_results_aggregate_level','8',null,null,null);
delete from $javascript_processes where process = 'output_model_compare_datatypes';
insert into $javascript_processes (javascript_filename,process) values ('model_results_compare_datatype_process.js','output_model_compare_datatypes');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('begin_date',null,null,'y','10');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('compare_station|compare_datatype',null,null,null,null);
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('end_date',null,null,'y','10');
insert into $javascript_files (javascript_filename,relative_source_directory_yn) values ('model_results_compare_datatype_process.js',null);
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('compare_output',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('compare_output','chart','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('compare_output','correlation','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('compare_output','scatter','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('compare_output','table','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('compare_output','text_file','5');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('model_results_aggregate_level',null,'aggregate_level');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','annually','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','monthly','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('model_results_aggregate_level','weekly','1');
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
