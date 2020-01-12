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
role_process="role_process"
role_process_set_member="role_process_set_member"
role_operation="role_operation"
(
cat << all_done
delete from $process where process = 'feeder_upload_statistics';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('feeder_upload_statistics','feeder_upload_statistics.sh \$process output_medium',null,null,null,null,null,'reconcile',null);
insert into $role_process (role,process) values ('supervisor','feeder_upload_statistics');
delete from $process_parameter where process = 'feeder_upload_statistics';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('feeder_upload_statistics','null','null','null','output_medium','1',null,null,null,null);
delete from $javascript_processes where process = 'feeder_upload_statistics';
delete from $process_generic_output where process = 'feeder_upload_statistics';
insert into $process_groups (process_group) values ('reconcile');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('output_medium',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','spreadsheet','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','table','2');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','text_file','3');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
