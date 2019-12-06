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

process="process"
operation="operation"
process_parameter="process_parameter"
javascript_processes="javascript_processes"
process_generic_output="process_generic_output"
javascript_files="javascript_files"
prompt="prompt"
process_groups="process_groups"
drop_down_prompt="drop_down_prompt"
drop_down_prompt_data="drop_down_prompt_data"
(
cat << all_done
delete from $process where process = 'load_cr10_single_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_cr10_single_file','load_cr10_single \$process filename station change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_cr10_single_file';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_cr10_single_file','null','null','change_existing_data_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_cr10_single_file','null','null','execute_yn','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_cr10_single_file','null','null','filename','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_cr10_single_file','station','null','null','null','2',null,null,null,null);
delete from $javascript_processes where process = 'load_cr10_single_file';
delete from $process_generic_output where process = 'load_cr10_single_file';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_YSI_data';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_YSI_data','load_ysi_data \$process filename station begin_date begin_time end_date end_time n change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_YSI_data';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','null','null','begin_date','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','null','null','begin_time','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','null','null','change_existing_data_yn','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','null','null','end_date','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','null','null','end_time','null','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','null','null','execute_yn','null','8',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','null','null','filename','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_YSI_data','station','null','null','null','2',null,null,'station_list',null);
delete from $javascript_processes where process = 'load_YSI_data';
delete from $process_generic_output where process = 'load_YSI_data';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_time','Format HHMM',null,null,'4');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_time','Format HHMM',null,null,'4');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = 'station_list';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('station_list','station_list.sh \$application \$login_name station \$where',null,null,null,null,null,null,null);
delete from $process where process = '';
delete from $process where process = 'load_EXO_data';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_EXO_data','load_ysi_data \$process filename station begin_date begin_time end_date end_time y change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_EXO_data';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','begin_date','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','begin_time','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','change_existing_data_yn','null','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','end_date','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','end_time','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','execute_yn','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_EXO_data','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_EXO_data';
delete from $process_generic_output where process = 'load_EXO_data';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_time','Format HHMM',null,null,'4');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_time','Format HHMM',null,null,'4');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_satlink2_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_satlink2_file','sl2_upload \$process station filename change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_satlink2_file';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_satlink2_file','null','null','change_existing_data_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_satlink2_file','null','null','execute_yn','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_satlink2_file','null','null','filename','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_satlink2_file','station','null','null','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_satlink2_file';
delete from $process_generic_output where process = 'load_satlink2_file';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_satlink3_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_satlink3_file','sl3_upload \$process station filename change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_satlink3_file';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_satlink3_file','null','null','change_existing_data_yn','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_satlink3_file','null','null','execute_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_satlink3_file','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_satlink3_file';
delete from $process_generic_output where process = 'load_satlink3_file';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_biscayne_YSI_data';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_biscayne_YSI_data','load_biscayne_ysi_data \$process filename biscayne_station begin_date begin_time end_date end_time execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_biscayne_YSI_data';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','begin_date','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','begin_time','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','biscayne_station','null','2',null,null,'station_list_bisc',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','change_existing_data_yn','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','end_date','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','end_time','null','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','execute_yn','null','8',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_biscayne_YSI_data','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_biscayne_YSI_data';
delete from $process_generic_output where process = 'load_biscayne_YSI_data';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_time','Format HHMM',null,null,'4');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('biscayne_station',null,null,null,null);
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date','Format:  YYYY-MM-DD',null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_time','Format HHMM',null,null,'4');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = 'station_list_bisc';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('station_list_bisc','station_list_bisc.sh \$application',null,null,null,null,null,null,null);
delete from $process where process = '';
delete from $process where process = 'load_SFWMD_single_file';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_SFWMD_single_file','load_sfwmd_single_file \$process filename execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_SFWMD_single_file';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_single_file','null','null','change_existing_data_yn','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_single_file','null','null','execute_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_single_file','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_SFWMD_single_file';
delete from $process_generic_output where process = 'load_SFWMD_single_file';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'shef_upload';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('shef_upload','shef_upload \$process shef_filename change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'shef_upload';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('shef_upload','null','null','change_existing_data_yn','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('shef_upload','null','null','execute_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('shef_upload','null','null','shef_filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'shef_upload';
delete from $process_generic_output where process = 'shef_upload';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('shef_filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_CR300';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_CR300','load_cr300_data \$process filename station change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_CR300';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_CR300','null','null','change_existing_data_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_CR300','null','null','execute_yn','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_CR300','null','null','filename','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_CR300','station','null','null','null','2',null,null,null,null);
delete from $javascript_processes where process = 'load_CR300';
delete from $process_generic_output where process = 'load_CR300';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'LT_upload';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('LT_upload','LT_upload \$process station filename change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'LT_upload';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('LT_upload','null','null','change_existing_data_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('LT_upload','null','null','execute_yn','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('LT_upload','null','null','filename','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('LT_upload','station','null','null','null','1',null,null,null,null);
delete from $javascript_processes where process = 'LT_upload';
delete from $process_generic_output where process = 'LT_upload';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_SFWMD_breakpoint_data';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_SFWMD_breakpoint_data','load_sfwmd_breakpoint_data \$process filename change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_SFWMD_breakpoint_data';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_breakpoint_data','null','null','change_existing_data_yn','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_breakpoint_data','null','null','execute_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_SFWMD_breakpoint_data','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_SFWMD_breakpoint_data';
delete from $process_generic_output where process = 'load_SFWMD_breakpoint_data';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_turkey_point';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('load_turkey_point','load_turkey_point \$process station filename change_existing_data_yn execute_yn',null,null,null,null,'load',null,null);
delete from $process_parameter where process = 'load_turkey_point';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_turkey_point','null','null','change_existing_data_yn','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_turkey_point','null','null','execute_yn','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_turkey_point','null','null','filename','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_turkey_point','station','null','null','null','2',null,null,null,null);
delete from $javascript_processes where process = 'load_turkey_point';
delete from $process_generic_output where process = 'load_turkey_point';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('change_existing_data_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('load');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
