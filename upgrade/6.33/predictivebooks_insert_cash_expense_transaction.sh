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

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
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
role_process=`get_table_name $application role_process`
role_process_set_member=`get_table_name $application role_process_set_member`
role_operation=`get_table_name $application role_operation`
(
cat << all_done
delete from $process where process = 'insert_cash_expense_transaction';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('insert_cash_expense_transaction','insert_cash_transaction.sh \$process full_name street_address transaction_date account transaction_amount check_number memo fund',null,null,null,null,null,'reconcile',null);
insert into $role_process (role,process) values ('supervisor','insert_cash_expense_transaction');
delete from $process_parameter where process = 'insert_cash_expense_transaction';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','account','null','null','null','2',null,null,'populate_expense_account',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','entity','null','null','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','check_number','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','memo','null','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','transaction_amount','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('insert_cash_expense_transaction','null','null','transaction_date','null','3',null,null,null,null);
delete from $javascript_processes where process = 'insert_cash_expense_transaction';
delete from $process_generic_output where process = 'insert_cash_expense_transaction';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('check_number',null,null,null,'10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('memo',null,null,null,'60');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_amount',null,null,null,'10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transaction_date',null,null,'y','10');
insert into $process_groups (process_group) values ('reconcile');
delete from $process where process = 'populate_expense_account';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('populate_expense_account','populate_expense_account.sh',null,null,null,null,null,null,null);
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
