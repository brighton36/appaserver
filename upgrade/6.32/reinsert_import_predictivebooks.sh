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

if [ "$application" != "template" ]
then
	exit 1
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
delete from $process where process = 'import_predictivebooks';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('import_predictivebooks','import_predictivebooks_process.sh \$login \$process module opening_cash_balance cash_account_name equity_account_name execute_yn','This process imports the PredictiveBooks application.',null,null,null,'y','manipulate',null);
delete from $process_parameter where process = 'import_predictivebooks';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('import_predictivebooks','null','null','cash_account_name','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('import_predictivebooks','null','null','equity_account_name','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('import_predictivebooks','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('import_predictivebooks','null','null','null','predictivebooks_module','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('import_predictivebooks','null','null','opening_cash_balance','null','2',null,null,null,null);
delete from $javascript_processes where process = 'import_predictivebooks';
delete from $process_generic_output where process = 'import_predictivebooks';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('cash_account_name',null,null,null,'60');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('equity_account_name',null,null,null,'60');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('opening_cash_balance',null,null,null,'14');
insert into $process_groups (process_group) values ('manipulate');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('predictivebooks_module',null,'module');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('predictivebooks_module','nonprofit','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('predictivebooks_module','personal','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('predictivebooks_module','professional','2');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
