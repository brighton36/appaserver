#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != timriley -a		\
     "$applicaiion" != eves -a			\
     "$application" != appahost ]
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

(
cat << all_done
delete from $folder where folder = 'subsidiary_transaction';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,notepad,post_change_javascript,html_help_file_anchor,post_change_process,no_initial_capital_yn,row_access_count,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory) values ('subsidiary_transaction','prompt','5',null,null,null,null,null,null,null,null,null,null,null,null,'static',null);
delete from $relation where folder = 'subsidiary_transaction';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('subsidiary_transaction','account','credit_account',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('subsidiary_transaction','account','debit_account',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('subsidiary_transaction','folder','debit_account_folder',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('subsidiary_transaction','folder_attribute','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('folder','text','35',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'attribute';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('attribute','text','60',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'debit_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('debit_account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'debit_account_folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('debit_account_folder','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'credit_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('credit_account','text','60',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'subsidiary_transaction';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subsidiary_transaction','attribute',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subsidiary_transaction','credit_account',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subsidiary_transaction','debit_account',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subsidiary_transaction','debit_account_folder',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subsidiary_transaction','folder','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('subsidiary_transaction','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('subsidiary_transaction','supervisor','update');
delete from $javascript_folders where folder = 'subsidiary_transaction';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('subsidiary_transaction','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('subsidiary_transaction','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
