#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != hydrology ]
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
delete from $process where process = 'output_annual_comparison';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,post_change_javascript) values ('output_annual_comparison','output_annual_comparison \$application \$session \$process station datatype begin_month_day end_month_day begin_year end_year aggregate_statistic output_medium',null,null,null,null,'output',null);
delete from $process_parameter where process = 'output_annual_comparison';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_annual_comparison','null','null','begin_month_day','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_annual_comparison','null','null','begin_year','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_annual_comparison','null','null','end_month_day','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_annual_comparison','null','null','end_year','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_annual_comparison','null','null','null','aggregate_statistic','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_annual_comparison','null','null','null','annual_comparison_output_medium','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('output_annual_comparison','station_datatype','null','null','null','1',null,null,null,null);
delete from $javascript_processes where process = 'output_annual_comparison';
delete from $process_generic_output where process = 'output_annual_comparison';
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('begin_month_day',null,'Format MM-DD',null,'5');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('begin_year',null,null,null,'4');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('end_month_day',null,'Format MM-DD',null,'5');
insert into $prompt (prompt,upload_filename_yn,hint_message,date_yn,input_width) values ('end_year',null,null,null,'4');
insert into $process_groups (process_group) values ('output');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregate_statistic','!Use SUM for acre-feet and kaf',null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','average','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','maximum','5');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','median','4');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','minimum','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','non_zero_percent','8');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','range','6');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','standard_deviation','7');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregate_statistic','sum','2');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('annual_comparison_output_medium',null,'output_medium');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('annual_comparison_output_medium','chart','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('annual_comparison_output_medium','spreadsheet','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('annual_comparison_output_medium','table','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('annual_comparison_output_medium','text_file','4');
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
