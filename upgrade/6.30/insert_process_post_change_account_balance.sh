#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != timriley -a		\
     "$application" != eves -a			\
     "$application" != capitolpops -a		\
     "$application" != appahost ]
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
delete from $process where process = 'post_change_account_balance';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('post_change_account_balance','post_change_account_balance \$application fund full_name street_address account_number date_time \$state preupdate_full_name preupdate_street_address preupdate_account_number',null,null,null,null,null,null,null);
delete from $process_parameter where process = 'post_change_account_balance';
delete from $javascript_processes where process = 'post_change_account_balance';
delete from $process_generic_output where process = 'post_change_account_balance';
all_done
) | sql.e 2>&1 | grep -iv duplicate

(
cat << all_done2
delete from $process where process = 'populate_investment_account';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('populate_investment_account','populate_investment_account.sh \$application \$where',null,null,null,null,null,null,null);
delete from $process_parameter where process = 'populate_investment_account';
delete from $javascript_processes where process = 'populate_investment_account';
delete from $process_generic_output where process = 'populate_investment_account';
all_done2
) | sql.e 2>&1 | grep -iv duplicate

exit 0
