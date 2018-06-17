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
(
cat << all_done
delete from $process where process = 'fix_orphans';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('fix_orphans','fix_orphans \$process orphans_folder delete_yn really_yn','This process traverses the many-to-one relationships for a folder. It inserts the missing primary keys that contain foreign keys in the selected folder. Warning: this process could take a long time to run.',null,null,null,'y',null,null);
delete from $process_parameter where process = 'fix_orphans';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('fix_orphans','null','null','delete_yn','null','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('fix_orphans','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('fix_orphans','null','null','orphans_folder','null','1',null,null,'orphans_process_folder_list',null);
delete from $javascript_processes where process = 'fix_orphans';
delete from $process_generic_output where process = 'fix_orphans';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('delete_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('orphans_folder',null,null,null,null);
delete from $process where process = '';
delete from $process where process = 'orphans_process_folder_list';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('orphans_process_folder_list','orphans_process_folder_list.sh \$application',null,null,null,null,'y',null,null);
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
