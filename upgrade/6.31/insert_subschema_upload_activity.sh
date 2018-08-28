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

folder_attribute_exists.sh $application bank_uploads bank_date

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
delete from $folder where folder = 'activity';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,exclude_application_export_yn,no_initial_capital_yn,lookup_email_output_yn,html_help_file_anchor,appaserver_yn,data_directory,index_directory) values ('activity','table','5','activity',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'activity';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('role_activity','activity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'activity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('activity','text','30',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'activity';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('activity','activity','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('activity','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('activity','supervisor','update');
delete from $javascript_folders where folder = 'activity';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('activity');
insert into $role_operation (folder,role,operation) values ('activity','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('activity','supervisor','detail');
delete from $folder where folder = 'bank_upload_activity_work';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,exclude_application_export_yn,no_initial_capital_yn,lookup_email_output_yn,html_help_file_anchor,appaserver_yn,data_directory,index_directory) values ('bank_upload_activity_work','prompt','5','activity',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_activity_work';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','role_activity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','50',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'begin_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date_time','current_date_time','16',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date_time','date_time','16',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'role';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('role','text','25',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'activity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('activity','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('work_hours','float','6','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_activity_work';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','activity',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','bank_upload_date_time',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','begin_work_date_time','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','end_work_date_time',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','login_name','1',null,null,'y','y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','role',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_activity_work','work_hours',null,'5',null,'y','y',null,null,null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('bank_upload_activity_work','supervisor','update');
delete from $javascript_folders where folder = 'bank_upload_activity_work';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('activity');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload_activity_work','supervisor','detail');
delete from $folder where folder = 'bank_upload_archive';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,exclude_application_export_yn,no_initial_capital_yn,lookup_email_output_yn,html_help_file_anchor,appaserver_yn,data_directory,index_directory) values ('bank_upload_archive','prompt','5','bank_upload',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_archive';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_archive','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_description','text','140',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sequence_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sequence_number','integer','9',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_amount','float','10','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_running_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_running_balance','float','12','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_archive';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_date','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_description','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_running_balance',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_upload_date_time',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','sequence_number',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_archive','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_archive','supervisor','lookup');
delete from $javascript_folders where folder = 'bank_upload_archive';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('bank_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_archive','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_archive','supervisor','detail');
delete from $folder where folder = 'bank_upload_event';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,exclude_application_export_yn,no_initial_capital_yn,lookup_email_output_yn,html_help_file_anchor,appaserver_yn,data_directory,index_directory) values ('bank_upload_event','prompt','5','bank_upload',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_event';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_archive','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_upload_filename';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_filename','http_filename','80',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'completed_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('completed_date_time','date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_event';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_date_time','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_filename',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','completed_date_time',null,'2',null,null,null,null,null,null,null);
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
delete from $folder where folder = 'role_activity';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,exclude_application_export_yn,no_initial_capital_yn,lookup_email_output_yn,html_help_file_anchor,appaserver_yn,data_directory,index_directory) values ('role_activity','prompt','5','activity',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'role_activity';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('role_activity','activity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('role_activity','role','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,omit_lookup_before_drop_down_yn,copy_common_attributes_yn,hint_message) values ('bank_upload_activity_work','role_activity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'role';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('role','text','25',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'activity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('activity','text','30',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'role_activity';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('role_activity','activity','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,lookup_required_yn,insert_required_yn) values ('role_activity','role','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('role_activity','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('role_activity','supervisor','update');
delete from $javascript_folders where folder = 'role_activity';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('activity');
insert into $role_operation (folder,role,operation) values ('role_activity','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('role_activity','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
