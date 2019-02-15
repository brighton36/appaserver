#!/bin/sh

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

if [ "$application" != waterquality ]
then
	exit 0
fi

process_set=`get_table_name $application process_set`
process_set_parameter=`get_table_name $application process_set_parameter`
javascript_process_sets=`get_table_name $application javascript_process_sets`
javascript_files=`get_table_name $application javascript_files`
prompt=`get_table_name $application prompt`
process=`get_table_name $application process`
drop_down_prompt=`get_table_name $application drop_down_prompt`
drop_down_prompt_data=`get_table_name $application drop_down_prompt_data`
(
cat << all_done
delete from $process_set where process_set = 'output_results_per_project';
insert into $process_set (process_set,notepad,html_help_file_anchor,post_change_javascript,prompt_display_text,prompt_display_bottom_yn,process_group,preprompt_help_text) values ('output_results_per_project',null,null,'post_change_output_results(this)','output_medium','y','output_results',null);
delete from $process_set_parameter where process_set = 'output_results_per_project';
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_project','null','null','begin_date','null','3',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_project','null','null','end_date','null','4',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_project','null','null','null','aggregate_level','5',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_project','null','null','null','aggregate_statistic','6',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_project','parameter_unit','null','null','null','2',null,'y',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_project','station_parameter','null','null','null','1','n','n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_project','water_project_station','null','null','null','1','y','y',null,null);
delete from $javascript_process_sets where process_set = 'output_results_per_project';
insert into $javascript_process_sets (javascript_filename,process_set) values ('output_results_process_set.js','output_results_per_project');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date',null,null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date',null,null,'y','10');
delete from $process where process = 'output_results_exceedance';
delete from $process where process = 'output_results_exceedance_chart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exceedance_chart','generic_output_exceedance \$application \$login_name \$process_set chart \$dictionary',null,null,null,null,null,'exceedance_chart',null);
delete from $process where process = 'output_results_exceedance_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exceedance_table','generic_output_exceedance \$application \$login_name \$process_set table \$dictionary',null,null,null,null,null,'exceedance_table',null);
delete from $process where process = 'output_results_exception';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exception','output_results_exception \$application \$process_set \$process begin_date end_date \$dictionary',null,null,null,null,null,'exception',null);
delete from $process where process = 'output_results_exception_free';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exception_free','output_results_exception_free ignored \$process_set \$process begin_date end_date \$dictionary',null,null,null,null,null,'exception_free',null);
delete from $process where process = 'output_results_googlechart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_googlechart','generic_googlechart \$application \$process_set \$dictionary',null,null,null,null,null,'googlechart',null);
delete from $process where process = 'output_results_gracechart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_gracechart','generic_gracechart \$application \$role \$process_set \$dictionary',null,null,null,null,null,'gracechart',null);
delete from $process where process = 'output_results_high_average_low';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_high_average_low','generic_high_average_low \$application \$role \$process_set \$dictionary',null,null,null,null,null,'high_average_low',null);
delete from $process where process = 'output_results_histogram';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_histogram','generic_histogram \$application \$process_set \$dictionary',null,null,null,null,null,'histogram',null);
delete from $process where process = 'output_results_spreadsheet';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_spreadsheet','generic_output_text_file \$application \$process_set spreadsheet \$dictionary',null,null,null,null,null,'spreadsheet',null);
delete from $process where process = 'output_results_statistics';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_statistics','generic_statistics_report \$application  \$process_set \$dictionary',null,null,null,null,null,'statistics_report',null);
delete from $process where process = 'output_results_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_table','generic_output_text_file \$application \$process_set table \$dictionary',null,null,null,null,null,'table',null);
delete from $process where process = 'output_results_text_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_text_file','generic_output_text_file \$application \$process_set text_file \$dictionary',null,null,null,null,null,'text_file',null);
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_level',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','annually','9');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','monthly','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','real_time','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','weekly','5');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_statistic',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','average','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','maximum','5');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','median','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','minimum','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','non_zero_percent','8');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','range','6');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','standard_deviation','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','sum','2');
insert into $javascript_files (javascript_filename,relative_source_directory_yn) values ('output_results_process_set.js','y');
delete from $process_set where process_set = 'output_results_per_station';
insert into $process_set (process_set,notepad,html_help_file_anchor,post_change_javascript,prompt_display_text,prompt_display_bottom_yn,process_group,preprompt_help_text) values ('output_results_per_station',null,null,'post_change_output_results(this)','output_medium','y','output_results',null);
delete from $process_set_parameter where process_set = 'output_results_per_station';
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_station','null','null','begin_date','null','3',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_station','null','null','end_date','null','4',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_station','null','null','null','aggregate_level','5',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_station','null','null','null','aggregate_statistic','6',null,'n',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_station','parameter_unit','null','null','null','2',null,'y',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_station','station','null','null','null','1','y','y',null,null);
insert into $process_set_parameter (process_set,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_results_per_station','station_parameter','null','null','null','1','n','n',null,null);
delete from $javascript_process_sets where process_set = 'output_results_per_station';
insert into $javascript_process_sets (javascript_filename,process_set) values ('output_results_process_set.js','output_results_per_station');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date',null,null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date',null,null,'y','10');
delete from $process where process = 'output_results_exceedance';
delete from $process where process = 'output_results_exceedance_chart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exceedance_chart','generic_output_exceedance \$application \$login_name \$process_set chart \$dictionary',null,null,null,null,null,'exceedance_chart',null);
delete from $process where process = 'output_results_exceedance_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exceedance_table','generic_output_exceedance \$application \$login_name \$process_set table \$dictionary',null,null,null,null,null,'exceedance_table',null);
delete from $process where process = 'output_results_exception';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exception','output_results_exception \$application \$process_set \$process begin_date end_date \$dictionary',null,null,null,null,null,'exception',null);
delete from $process where process = 'output_results_exception_free';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_exception_free','output_results_exception_free ignored \$process_set \$process begin_date end_date \$dictionary',null,null,null,null,null,'exception_free',null);
delete from $process where process = 'output_results_googlechart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_googlechart','generic_googlechart \$application \$process_set \$dictionary',null,null,null,null,null,'googlechart',null);
delete from $process where process = 'output_results_gracechart';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_gracechart','generic_gracechart \$application \$role \$process_set \$dictionary',null,null,null,null,null,'gracechart',null);
delete from $process where process = 'output_results_high_average_low';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_high_average_low','generic_high_average_low \$application \$role \$process_set \$dictionary',null,null,null,null,null,'high_average_low',null);
delete from $process where process = 'output_results_histogram';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_histogram','generic_histogram \$application \$process_set \$dictionary',null,null,null,null,null,'histogram',null);
delete from $process where process = 'output_results_spreadsheet';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_spreadsheet','generic_output_text_file \$application \$process_set spreadsheet \$dictionary',null,null,null,null,null,'spreadsheet',null);
delete from $process where process = 'output_results_statistics';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_statistics','generic_statistics_report \$application  \$process_set \$dictionary',null,null,null,null,null,'statistics_report',null);
delete from $process where process = 'output_results_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_table','generic_output_text_file \$application \$process_set table \$dictionary',null,null,null,null,null,'table',null);
delete from $process where process = 'output_results_text_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,appaserver_yn,process_group,process_set_display,preprompt_help_text) values ('output_results_text_file','generic_output_text_file \$application \$process_set text_file \$dictionary',null,null,null,null,null,'text_file',null);
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_level',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','annually','9');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','monthly','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','real_time','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','weekly','5');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_statistic',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','average','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','maximum','5');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','median','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','minimum','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','non_zero_percent','8');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','range','6');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','standard_deviation','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','sum','2');
insert into $javascript_files (javascript_filename,relative_source_directory_yn) values ('output_results_process_set.js','y');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
