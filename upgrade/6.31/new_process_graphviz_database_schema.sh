#!/bin/sh
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
delete from $process where process = 'graphviz_database_schema';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('graphviz_database_schema','graphviz_database_schema_process.sh \$process appaserver_yn',null,null,null,null,null,'documentation',null);
insert into $role_process (role,process) values ('supervisor','graphviz_database_schema');
insert into $role_process (role,process) values ('system','graphviz_database_schema');
delete from $process_parameter where process = 'graphviz_database_schema';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('graphviz_database_schema','null','null','appaserver_yn','null','1',null,null,null,null);
delete from $javascript_processes where process = 'graphviz_database_schema';
delete from $process_generic_output where process = 'graphviz_database_schema';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('appaserver_yn',null,null,null,'1');
insert into $process_groups (process_group) values ('documentation');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
