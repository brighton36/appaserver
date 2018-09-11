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
# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
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
delete from $folder where folder = 'bank_upload_event';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,notepad,post_change_process,post_change_javascript,html_help_file_anchor,no_initial_capital_yn,exclude_application_export_yn,appaserver_yn,lookup_email_output_yn,data_directory,index_directory) values ('bank_upload_event','prompt','5','bank_upload',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_event';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_event','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_activity_work','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_archive','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'completed_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('completed_date_time','date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_filename';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_filename','http_filename','80',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'file_sha256sum';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('file_sha256sum','text','64',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_event';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_date_time','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_filename',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','completed_date_time',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','file_sha256sum',null,'4',null,null,'y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','login_name',null,'1','y',null,null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_event','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_event','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('bank_upload_event','supervisor','update');
delete from $javascript_folders where folder = 'bank_upload_event';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('bank_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
