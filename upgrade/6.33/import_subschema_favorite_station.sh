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

if [ "$application" != "hydrology" -a "$application" != "audubon" ]
then
	exit 0
fi

folder=`get_table_name $application folder`
relation=`get_table_name $application relation`
attribute=`get_table_name $application attribute`
folder_attribute=`get_table_name $application folder_attribute`
role_folder=`get_table_name $application role_folder`
row_security_role_update=`get_table_name $application row_security_role_update`
subschemas=`get_table_name $application subschemas`
role_operation=`get_table_name $application role_operation`
javascript_folders=`get_table_name $application javascript_folders`
javascript_files=`get_table_name $application javascript_files`
process=`get_table_name $application process`
role=`get_table_name $application role`

(
cat << all_done
delete from $folder where folder = 'favorite_station';
insert into $folder (folder,form,lookup_email_output_yn,populate_drop_down_process,appaserver_yn,insert_rows_number,notepad,html_help_file_anchor,post_change_process,post_change_javascript,no_initial_capital_yn,row_access_count,tablespace,exclude_application_export_yn,lookup_before_drop_down_yn,data_directory,subschema,index_directory,create_view_statement) values ('favorite_station','prompt',null,null,null,'5',null,null,null,null,null,null,null,null,null,null,'station',null,null);
delete from $relation where folder = 'favorite_station';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('favorite_station','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('favorite_station','station','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'favorite_station_set_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('favorite_station_set_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','20',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'station';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('station','text','20',null,null,null,null,null,null,'n');
delete from $folder_attribute where folder = 'favorite_station';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('favorite_station','favorite_station_set_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('favorite_station','login_name','2',null,null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('favorite_station','station','3',null,null,null,null,null,null,null,null);
insert into $role_folder (role,folder,permission) values ('hydrologist','favorite_station','insert');
insert into $role_folder (role,folder,permission) values ('hydrologist','favorite_station','update');
insert into $role_folder (role,folder,permission) values ('supervisor','favorite_station','insert');
insert into $role_folder (role,folder,permission) values ('supervisor','favorite_station','update');
insert into $role_folder (role,folder,permission) values ('technician','favorite_station','insert');
insert into $role_folder (role,folder,permission) values ('technician','favorite_station','update');
delete from $javascript_folders where folder = 'favorite_station';
insert into $role (role,override_row_restrictions_yn,folder_count_yn) values ('hydrologist',null,null);
insert into $role (role,override_row_restrictions_yn,folder_count_yn) values ('supervisor','y','y');
insert into $role (role,override_row_restrictions_yn,folder_count_yn) values ('technician',null,null);
insert into $subschemas (subschema) values ('station');
insert into $role_operation (folder,role,operation) values ('favorite_station','hydrologist','delete');
insert into $role_operation (folder,role,operation) values ('favorite_station','hydrologist','detail');
insert into $role_operation (folder,role,operation) values ('favorite_station','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('favorite_station','supervisor','detail');
insert into $role_operation (folder,role,operation) values ('favorite_station','technician','delete');
insert into $role_operation (folder,role,operation) values ('favorite_station','technician','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
