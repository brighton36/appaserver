#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != audubon -a "$application" != hydrology ]
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
delete from $process where process = 'station_datatype_list';
insert into $process (process,command_line,notepad,html_help_file_anchor,post_change_javascript,process_set_display,appaserver_yn,process_group,preprompt_help_text) values ('station_datatype_list','station_datatype_list \$application \$login_name \$process station datatype plot_for_station_check_yn \$one2m_folder validation_required_yn',null,null,null,null,null,null,null);
delete from $process_parameter where process = 'station_datatype_list';
delete from $javascript_processes where process = 'station_datatype_list';
delete from $process_generic_output where process = 'station_datatype_list';
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
