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
delete from $process where process = 'output_exceedance_curve';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('output_exceedance_curve','exceedance_curve_output \$process station datatype begin_date end_date aggregate_level aggregate_statistic output_medium email_address units_converted horizontal_line_at_point','This process sorts measurements in descending order, then for each value it counts and ratios the number of values below it.','process_exceedance_curve_help.html',null,null,'output',null,'post_change_exceedance_curve(this)');
delete from $process_parameter where process = 'output_exceedance_curve';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','null','null','begin_date','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','null','null','end_date','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','null','null','horizontal_line_at_point','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','null','null','null','aggregate_level','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','null','null','null','aggregate_statistic','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','null','null','null','output_medium','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','station_datatype','null','null','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_exceedance_curve','units_converted','null','null','null','5',null,null,null,null);
delete from $javascript_processes where process = 'output_exceedance_curve';
insert into $javascript_processes (javascript_filename,process) values ('exceedance_curve_process.js','output_exceedance_curve');
delete from $process_generic_output where process = 'output_exceedance_curve';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('horizontal_line_at_point',null,null,null,'8');
insert into $process_groups (process_group) values ('output');
insert into $javascript_files (javascript_filename) values ('exceedance_curve_process.js');
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
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('output_medium',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','chart','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','spreadsheet','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','table','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','text_file','3');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
