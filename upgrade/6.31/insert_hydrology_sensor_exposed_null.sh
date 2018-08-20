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

if [ "$application" != "hydrology" ]
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
role_process=`get_table_name $application role_process`
role_process_set_member=`get_table_name $application role_process_set_member`
role_operation=`get_table_name $application role_operation`

(
cat << all_done
delete from $process where process = 'sensor_exposed_null';
insert into $process (process,command_line,notepad,html_help_file_anchor,process_set_display,appaserver_yn,process_group,preprompt_help_text,post_change_javascript) values ('sensor_exposed_null','sensor_exposed_null_output \$login_name \$process \$role station datatype from_measurement_date to_measurement_date from_measurement_time to_measurement_time threshold_value above_below chart_yn \$dictionary notes execute_yn','This process allows you to null out measurement values if a sensor inadvertently becomes exposed.',null,null,null,'manipulate',null,null);
insert into $role_process (role,process) values ('geo','sensor_exposed_null');
insert into $role_process (role,process) values ('hydrologist','sensor_exposed_null');
insert into $role_process (role,process) values ('nps_tech','sensor_exposed_null');
insert into $role_process (role,process) values ('supervisor','sensor_exposed_null');
insert into $role_process (role,process) values ('technican','sensor_exposed_null');
insert into $role_process (role,process) values ('technician','sensor_exposed_null');
delete from $process_parameter where process = 'sensor_exposed_null';
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','measurement_date','null','null','2',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','measurement_time','null','null','3',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','null','chart_yn','null','8',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','null','execute_yn','null','9',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','null','notes','null','6',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','null','null','above_below','5',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','null','override_destination_validation_requirement_yn','null','7',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','null','null','threshold_value','null','4',null,null,null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','station','null','null','null','1',null,'y',null,null);
insert into $process_parameter (process,folder,attribute,prompt,drop_down_prompt,display_order,drop_down_multi_select_yn,preprompt_yn,populate_drop_down_process,populate_helper_process) values ('sensor_exposed_null','station_datatype','null','null','null','1',null,null,null,null);
delete from $javascript_processes where process = 'sensor_exposed_null';
delete from $process_generic_output where process = 'sensor_exposed_null';
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('chart_yn',null,null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('execute_yn','Do you really want to execute this PROCESS?',null,null,'1');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('notes',null,null,null,'30');
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('override_destination_validation_requirement_yn','Normally, if any of the destination measurements are validated, then this process will not be performed. Setting this flag to Yes will force the process to be performed even if some destination measurements are validated.',null,null,null);
insert into $prompt (prompt,hint_message,upload_filename_yn,date_yn,input_width) values ('threshold_value',null,null,null,'10');
insert into $process_groups (process_group) values ('manipulate');
insert into $drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('above_below',null,null);
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('above_below','above','1');
insert into $drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('above_below','below','2');
delete from $process where process = '';
delete from $process where process = '';
all_done
) | sql.e 2>&1 | grep -iv duplicate

(
cat << all_done2
insert into measurement_update_method (measurement_update_method) values ( 'sensor_exposed_null' );

all_done2
) | sql.e 2>&1 | grep -iv duplicate

exit 0
