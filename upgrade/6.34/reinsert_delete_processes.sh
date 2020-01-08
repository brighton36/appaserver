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
delete from $process where process = 'delete';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('delete','delete_folder_row \$session \$login_name \$folder \$role \$primary_data_list n',null,null,null,null,'y',null,null);
delete from $operation where operation = 'delete';
insert into $operation (operation,output_yn) values ('delete','n');
delete from $process_parameter where process = 'delete';
delete from $javascript_processes where process = 'delete';
delete from $process_generic_output where process = 'delete';
delete from $process where process = 'delete_isa_only';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('delete_isa_only','delete_folder_row \$session \$login_name \$folder \$role \$primary_data_list y',null,null,null,null,'y',null,null);
delete from $operation where operation = 'delete_isa_only';
insert into $operation (operation,output_yn) values ('delete_isa_only',null);
delete from $process_parameter where process = 'delete_isa_only';
delete from $javascript_processes where process = 'delete_isa_only';
delete from $process_generic_output where process = 'delete_isa_only';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
