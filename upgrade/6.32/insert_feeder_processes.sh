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
delete from $process where process = 'automatic_transaction_assign_all';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('automatic_transaction_assign_all','automatic_transaction_assign.sh all \$process',null,null,null,null,null,'reconciliation',null);
insert into $role_process (role,process) values ('bookkeeper','automatic_transaction_assign_all');
insert into $role_process (role,process) values ('supervisor','automatic_transaction_assign_all');
delete from $process_parameter where process = 'automatic_transaction_assign_all';
delete from $javascript_processes where process = 'automatic_transaction_assign_all';
delete from $process_generic_output where process = 'automatic_transaction_assign_all';
insert into $process_groups (process_group) values ('reconciliation');
delete from $process where process = 'automatic_transaction_assign_one';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('automatic_transaction_assign_one','automatic_transaction_assign.sh one \$process',null,null,null,null,null,'reconciliation',null);
insert into $role_process (role,process) values ('bookkeeper','automatic_transaction_assign_one');
insert into $role_process (role,process) values ('supervisor','automatic_transaction_assign_one');
delete from $process_parameter where process = 'automatic_transaction_assign_one';
delete from $javascript_processes where process = 'automatic_transaction_assign_one';
delete from $process_generic_output where process = 'automatic_transaction_assign_one';
insert into $process_groups (process_group) values ('reconciliation');
delete from $process where process = 'direct_transaction_assign';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('direct_transaction_assign','bank_upload_transaction_assign.sh \$process bank_date bank_description full_name street_address transaction_date_time',null,null,null,null,null,'reconciliation',null);
insert into $role_process (role,process) values ('bookkeeper','direct_transaction_assign');
insert into $role_process (role,process) values ('supervisor','direct_transaction_assign');
delete from $process_parameter where process = 'direct_transaction_assign';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('direct_transaction_assign','bank_upload','null','null','null','1',null,null,'populate_bank_upload_pending',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('direct_transaction_assign','transaction','null','null','null','2',null,null,'populate_bank_upload_transaction_pending',null);
delete from $javascript_processes where process = 'direct_transaction_assign';
delete from $process_generic_output where process = 'direct_transaction_assign';
insert into $process_groups (process_group) values ('reconciliation');
delete from $process where process = 'populate_bank_upload_pending';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('populate_bank_upload_pending','populate_bank_upload_pending.sh',null,null,null,null,null,null,null);
delete from $process where process = 'populate_bank_upload_transaction_pending';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('populate_bank_upload_transaction_pending','populate_bank_upload_transaction_pending.sh',null,null,null,null,null,null,null);
delete from $process where process = '';
delete from $process where process = 'post_cash_expense_transaction';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('post_cash_expense_transaction','post_cash_expense_transaction \$process \$session \$role full_name street_address bank_date bank_description account memo table execute_yn',null,null,null,null,null,'reconciliation',null);
insert into $role_process (role,process) values ('bookkeeper','post_cash_expense_transaction');
insert into $role_process (role,process) values ('supervisor','post_cash_expense_transaction');
delete from $process_parameter where process = 'post_cash_expense_transaction';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('post_cash_expense_transaction','account','null','null','null','3',null,null,'populate_expense_account',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('post_cash_expense_transaction','bank_upload','null','null','null','1',null,null,'populate_bank_upload_pending',null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('post_cash_expense_transaction','entity','null','null','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('post_cash_expense_transaction','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('post_cash_expense_transaction','null','null','memo','null','5',null,null,null,null);
delete from $javascript_processes where process = 'post_cash_expense_transaction';
delete from $process_generic_output where process = 'post_cash_expense_transaction';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('memo',null,null,null,'60');
insert into $process_groups (process_group) values ('reconciliation');
delete from $process where process = 'populate_expense_account';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('populate_expense_account','populate_expense_account.sh',null,null,null,null,null,null,null);
delete from $process where process = 'populate_bank_upload_pending';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('populate_bank_upload_pending','populate_bank_upload_pending.sh',null,null,null,null,null,null,null);
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_cash_spreadsheet';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('load_cash_spreadsheet','load_bank_spreadsheet \$process \$login_name fund bank_of_america_checking filename 1 2 3 -1 4 transactions_only_yn execute_yn',null,null,null,null,null,'reconciliation',null);
insert into $role_process (role,process) values ('bookkeeper','load_cash_spreadsheet');
insert into $role_process (role,process) values ('supervisor','load_cash_spreadsheet');
delete from $process_parameter where process = 'load_cash_spreadsheet';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_cash_spreadsheet','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_cash_spreadsheet','null','null','filename','null','1',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_cash_spreadsheet','null','null','transactions_only_yn','null','3',null,null,null,null);
delete from $javascript_processes where process = 'load_cash_spreadsheet';
delete from $process_generic_output where process = 'load_cash_spreadsheet';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('transactions_only_yn',null,null,null,'1');
insert into $process_groups (process_group) values ('reconciliation');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'load_credit_spreadsheet';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('load_credit_spreadsheet','load_bank_spreadsheet \$process \$login_name fund bank_of_america_credit_card filename 1 3 5 -1 -1 transactions_only_yn execute_yn',null,null,null,null,null,'reconciliation',null);
insert into $role_process (role,process) values ('bookkeeper','load_credit_spreadsheet');
insert into $role_process (role,process) values ('supervisor','load_credit_spreadsheet');
delete from $process_parameter where process = 'load_credit_spreadsheet';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_credit_spreadsheet','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('load_credit_spreadsheet','null','null','filename','null','1',null,null,null,null);
delete from $javascript_processes where process = 'load_credit_spreadsheet';
delete from $process_generic_output where process = 'load_credit_spreadsheet';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('filename',null,'y',null,'100');
insert into $process_groups (process_group) values ('reconciliation');
delete from $process where process = '';
delete from $process where process = '';
delete from $process where process = 'transaction_balance_report';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('transaction_balance_report','transaction_balance_report \$process begin_date output_medium',null,null,null,null,null,'reconciliation',null);
insert into $role_process (role,process) values ('bookkeeper','transaction_balance_report');
insert into $role_process (role,process) values ('supervisor','transaction_balance_report');
delete from $process_parameter where process = 'transaction_balance_report';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('transaction_balance_report','null','null','begin_date','null','1',null,null,null,null);
delete from $javascript_processes where process = 'transaction_balance_report';
delete from $process_generic_output where process = 'transaction_balance_report';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date',null,null,'y','10');
insert into $process_groups (process_group) values ('reconciliation');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
