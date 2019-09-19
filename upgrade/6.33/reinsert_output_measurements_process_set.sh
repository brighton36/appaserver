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

process_set="process_set"
process_set_parameter="process_set_parameter"
javascript_process_sets="javascript_process_sets"
javascript_files="javascript_files"
prompt="prompt"
process="process"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
(
cat << all_done
delete from $process_set where process_set = 'output_measurements';
insert into $process_set (process_set,notepad,html_help_file_anchor,post_change_javascript,prompt_display_text,prompt_display_bottom_yn,process_group,preprompt_help_text) values ('output_measurements',null,'process_set_measurements_help.html','post_change_output_measurements(this)','output_medium','y','output',null);
delete from $process_set_parameter where process_set = 'output_measurements';
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','datatype','null','null','null','3','y','y',null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','favorite_station_set','null','null','null','2','y',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','accumulate_yn','null','null','7',null,null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','begin_date','null','null','2','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','days_to_average','null','null','12','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','email_address','null','null','9','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','end_date','null','null','3','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','null','null','aggregate_level','5','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','null','null','aggregate_statistic','6','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','null','null','validation_level','8','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','transmit_validation_info_yn','null','null','10','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','null','zulu_time_yn','null','null','13',null,null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','station','null','null','null','1','y','y',null,'station_map');
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','station_datatype','null','null','null','1','n',null,null,null);
insert into $process_set_parameter (process_set,folder,prompt,attribute,drop_down_prompt,display_order,preprompt_yn,drop_down_multi_select_yn,populate_drop_down_process,populate_helper_process) values ('output_measurements','units_converted','null','null','null','11','n',null,null,null);
delete from $javascript_process_sets where process_set = 'output_measurements';
insert into $javascript_process_sets (javascript_filename,process_set) values ('output_measurements_process_set.js','output_measurements');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('accumulate_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('days_to_average','Defaults to 30',null,null,'3');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('email_address','Optional',null,null,'50');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transmit_validation_info_yn',null,null,null,null);
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('zulu_time_yn',null,null,null,'1');
delete from $process where process = 'chart_high_average_low';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('chart_high_average_low','high_average_low_chart \$application \$session \$login_name \$role station datatype begin_date end_date aggregate_level email_address units_converted',null,null,null,null,'output',null,null);
delete from $process where process = 'chart_measurements';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('chart_measurements','custom_chart \$application \$role session station datatype begin_date end_date aggregate_level ymin ymax y_tick_major y_tick_minor gridlines_yn email_address validation_level aggregate_statistic symbols_yn plot_for_station_check_yn units_converted accumulate_yn',null,null,null,null,'output',null,null);
delete from $process where process = 'data_validation_form';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('data_validation_form','hydrology_validation_form \$application \$role \$session \$person station datatype begin_date end_date \$dictionary chart_yn',null,null,null,null,'manipulate',null,null);
delete from $process where process = 'histogram';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('histogram','hydrology_histogram \$application \$session station datatype begin_date end_date aggregate_level aggregate_statistic validation_level email_address units_converted',null,null,null,null,null,null,null);
delete from $process where process = 'output_daily_moving_statistic';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_daily_moving_statistic','daily_moving_statistic_output \$person \$session \$application \$role station datatype begin_date end_date validation_level days_to_average exceedance_format_yn aggregate_statistic output_medium units_converted',null,'process_moving_statistic_help.html',null,null,'output',null,'post_change_output_daily_moving_average(this)');
delete from $process where process = 'output_exceedance_curve';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_exceedance_curve','exceedance_curve_output \$process station datatype begin_date end_date aggregate_level aggregate_statistic output_medium email_address units_converted horizontal_line_at_point','This process sorts measurements in descending order, then for each value it counts and ratios the number of values below it.','process_exceedance_curve_help.html',null,null,'output',null,'post_change_exceedance_curve(this)');
delete from $process where process = 'output_measurement_googlecharts';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_measurement_googlecharts','output_measurement_googlecharts \$application \$process station datatype begin_date end_date aggregate_level validation_level aggregate_statistic units_converted accumulate_yn',null,null,'google_chart',null,null,null,null);
delete from $process where process = 'output_measurement_spreadsheet';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_measurement_spreadsheet','output_measurement_data \$application station datatype begin_date end_date email_address aggregate_level validation_level aggregate_statistic accumulate_yn transmit_validation_info_yn units_converted zulu_time_yn spreadsheet',null,null,'spreadsheet',null,null,null,null);
delete from $process where process = 'output_measurement_table';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_measurement_table','output_measurement_data \$application station datatype begin_date end_date email_address aggregate_level validation_level aggregate_statistic accumulate_yn transmit_validation_info_yn units_converted zulu_time_yn table',null,null,'table',null,'output',null,null);
delete from $process where process = 'quick_measurement_form';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('quick_measurement_form','hydrology_quick_measurement_form \$person \$application \$session \$dictionary',null,null,null,null,'manipulate',null,null);
delete from $process where process = 'shef_download';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('shef_download','shef_download \$person \$session \$application station datatype begin_date end_date validation_level email_address',null,null,null,null,'output',null,null);
delete from $process where process = 'statistics_report';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('statistics_report','hydrology_statistics_report \$application \$session \$login_name \$role station datatype begin_date end_date validation_level units_converted',null,null,null,null,null,null,null);
delete from $process where process = 'text_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('text_file','output_measurement_data \$application station datatype begin_date end_date email_address aggregate_level validation_level aggregate_statistic accumulate_yn transmit_validation_info_yn units_converted zulu_time_yn text_file',null,null,null,null,null,null,null);
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_level','daily, monthly, etc...',null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','annually','9');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','daily','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','half_hour','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','hourly','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','monthly','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','real_time','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_level','weekly','5');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_statistic','!Use SUM for acre-feet and kaf',null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','average','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','maximum','5');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','median','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','minimum','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','non_zero_percent','8');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','range','6');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','standard_deviation','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','sum','2');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('validation_level',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('validation_level','either','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('validation_level','provisional','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('validation_level','validated','2');
insert into $javascript_files (javascript_filename) values ('output_measurements_process_set.js');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
