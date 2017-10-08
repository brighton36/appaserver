#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != hydrology -a "$application" != audubon ]
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
delete from $process where process = 'output_measurement_googlecharts';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('output_measurement_googlecharts','output_measurement_googlecharts \$application \$process station datatype begin_date end_date aggregate_level validation_level aggregate_statistic units_converted accumulate_yn',null,null,null,'google_chart',null,null,null);
delete from $process_parameter where process = 'output_measurement_googlecharts';
delete from $javascript_processes where process = 'output_measurement_googlecharts';
delete from $process_generic_output where process = 'output_measurement_googlecharts';
insert into $role_process_set_member (process,process_set,role) values ('output_measurement_googlecharts','output_measurements','hydrologist');
insert into $role_process_set_member (process,process_set,role) values ('output_measurement_googlecharts','output_measurements','public');
insert into $role_process_set_member (process,process_set,role) values ('output_measurement_googlecharts','output_measurements','supervisor');
insert into $role_process_set_member (process,process_set,role) values ('output_measurement_googlecharts','output_measurements','technician');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
