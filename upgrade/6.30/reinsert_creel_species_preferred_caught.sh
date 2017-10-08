#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != creel ]
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
delete from $process where process = 'species_preferred/species_caught';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('species_preferred/species_caught','species_preferred_species_caught \$application \$process begin_date end_date family genus species fishing_purpose display_data',null,null,'post_change_species_preferred_species_caught(this)',null,null,'output',null);
delete from $process_parameter where process = 'species_preferred/species_caught';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('species_preferred/species_caught','families','null','null','null','8',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('species_preferred/species_caught','fishing_purposes','null','null','null','20',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('species_preferred/species_caught','genera','null','null','null','10',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('species_preferred/species_caught','null','null','begin_date','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('species_preferred/species_caught','null','null','end_date','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('species_preferred/species_caught','null','null','null','display_data_species_preferred_caught','99',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('species_preferred/species_caught','species','null','null','null','10',null,null,null,null);
delete from $javascript_processes where process = 'species_preferred/species_caught';
insert into $javascript_processes (javascript_filename,process) values ('species_preferred_species_caught.js','species_preferred/species_caught');
delete from $process_generic_output where process = 'species_preferred/species_caught';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('begin_date',null,null,'y','10');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('end_date',null,null,'y','10');
insert into $process_groups (process_group) values ('output');
insert into $javascript_files (javascript_filename) values ('species_preferred_species_caught.js');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('display_data_species_preferred_caught','Horizontally is the original behavior -- the activity data grows with new columns to the right. Vertically is an easy format to import into other applications.','display_data');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('display_data_species_preferred_caught','filtered','3');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('display_data_species_preferred_caught','horizontally','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('display_data_species_preferred_caught','vertically','2');
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
