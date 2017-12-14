#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" = ignored ]
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
delete from $folder where folder = 'account';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('account','prompt','5','ledger',null,null,'populate_account',null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'account';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('account','subclassification','null',null,null,null,null,null,null,null,null,'y',null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('contra_account','account','contra_to_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('contra_account','account','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','account','cost_of_goods_sold_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','account','inventory_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','account','null',null,null,null,null,'y',null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('liability_account_entity','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','account','credit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','account','debit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('service_purchase','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('supply','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('tax_form_line_account','account','null','1',null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'subclassification';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('subclassification','text','35',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'hard_coded_account_key';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('hard_coded_account_key','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'account';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('account','account','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('account','hard_coded_account_key',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('account','subclassification',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('account','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('account','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('account','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('account','supervisor','update');
delete from $javascript_folders where folder = 'account';
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('account','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('account','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('account','bookkeeper','ledger_propagate');
insert into $role_operation (folder,role,operation) values ('account','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('account','supervisor','detail');
insert into $role_operation (folder,role,operation) values ('account','supervisor','ledger_propagate');
delete from $folder where folder = 'bank_upload';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('bank_upload','prompt','5','ledger',null,null,null,null,null,null,'post_change_bank_upload( ''\$state'', \$row )','post_change_bank_upload',null,null,null,null);
delete from $relation where folder = 'bank_upload';
delete from $attribute where attribute = 'bank_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_description','text','140',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sequence_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sequence_number','integer','9',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_amount','float','10','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_running_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_running_balance','float','12','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'posted_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('posted_yn','text','1',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_date','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_description','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','bank_running_balance',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','posted_yn',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload','sequence_number',null,'1',null,'y',null,null,'n',null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('bank_upload','supervisor','update');
delete from $javascript_folders where folder = 'bank_upload';
insert into $javascript_folders (javascript_filename,folder) values ('post_change_bank_upload.js','bank_upload');
delete from $javascript_files where javascript_filename = 'post_change_bank_upload.js';
insert into $javascript_files (javascript_filename) values ('post_change_bank_upload.js');
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('bank_upload','supervisor','delete');
delete from $folder where folder = 'contra_account';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('contra_account','prompt','5','ledger',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'contra_account';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('contra_account','account','contra_to_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('contra_account','account','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'contra_to_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('contra_to_account','text','60',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'contra_account';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('contra_account','account','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('contra_account','contra_to_account',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('contra_account','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('contra_account','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('contra_account','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('contra_account','supervisor','update');
delete from $javascript_folders where folder = 'contra_account';
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('contra_account','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('contra_account','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('contra_account','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('contra_account','supervisor','detail');
delete from $folder where folder = 'customer';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('customer','prompt','5','entity',null,'n',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'customer';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sales_tax_exempt_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sales_tax_exempt_yn','text','1',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'customer';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer','sales_tax_exempt_yn',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('customer','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('customer','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('customer','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('customer','supervisor','update');
delete from $javascript_folders where folder = 'customer';
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('customer','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('customer','bookkeeper','delete_isa_only');
insert into $role_operation (folder,role,operation) values ('customer','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('customer','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('customer','supervisor','delete_isa_only');
insert into $role_operation (folder,role,operation) values ('customer','supervisor','detail');
delete from $folder where folder = 'customer_payment';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('customer_payment','prompt','1','sale',null,null,null,null,null,null,null,'post_change_customer_payment',null,null,null,null);
delete from $relation where folder = 'customer_payment';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_payment','customer_sale','null',null,null,null,null,null,null,'y',null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_payment','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payment_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payment_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payment_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payment_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'check_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('check_number','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'customer_payment';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_payment','check_number',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_payment','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_payment','payment_amount',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_payment','payment_date_time','8',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_payment','sale_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_payment','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_payment','transaction_date_time',null,'3',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('customer_payment','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('customer_payment','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('customer_payment','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('customer_payment','supervisor','update');
delete from $javascript_folders where folder = 'customer_payment';
insert into $subschemas (subschema) values ('sale');
insert into $role_operation (folder,role,operation) values ('customer_payment','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('customer_payment','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('customer_payment','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('customer_payment','supervisor','detail');
delete from $folder where folder = 'customer_sale';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('customer_sale','prompt','1','sale',null,'y',null,null,null,null,null,'post_change_customer_sale',null,null,null,null);
delete from $relation where folder = 'customer_sale';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_sale','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_payment','customer_sale','null',null,null,null,null,null,null,'y',null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service_sale','customer_sale','null','2',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service_sale','customer_sale','null','5',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_sale','customer_sale','null','1',null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'shipping_revenue';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('shipping_revenue','float','8','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'sum_extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sum_extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'sales_tax';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sales_tax','float','7','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'invoice_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('invoice_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'total_payment';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('total_payment','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'amount_due';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('amount_due','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'completed_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('completed_date_time','date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'uncollectible_writeoff_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('uncollectible_writeoff_date','date','10',null,'Remember also to insert:<table><th>Account<th>Debit<th>Credit<tr><td>Loss<td>\$amount<tr><td>AR<td><td>\$amount</table>',null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'customer_sale';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','amount_due',null,'15',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','completed_date_time',null,'16',null,'n',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','invoice_amount',null,'13',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','sales_tax',null,'12',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','sale_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','shipping_revenue',null,'10',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','sum_extension',null,'11',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','total_payment',null,'14',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','transaction_date_time',null,'18',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('customer_sale','uncollectible_writeoff_date',null,'17',null,'y',null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('customer_sale','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('customer_sale','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('customer_sale','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('customer_sale','supervisor','update');
delete from $javascript_folders where folder = 'customer_sale';
insert into $subschemas (subschema) values ('sale');
insert into $role_operation (folder,role,operation) values ('customer_sale','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('customer_sale','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('customer_sale','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('customer_sale','supervisor','detail');
delete from $folder where folder = 'day';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('day','table','7','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'day';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','day','payroll_beginning_day',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'day';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('day','text','9',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'day';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('day','day','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('day','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('day','supervisor','update');
delete from $javascript_folders where folder = 'day';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('day','supervisor','delete');
delete from $folder where folder = 'depreciation';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('depreciation','prompt','5','purchase',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'depreciation';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','fixed_asset_purchase','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','depreciation','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'serial_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('serial_number','text','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'units_produced';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('units_produced','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_amount','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'depreciation';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','asset_name',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','depreciation_amount',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','depreciation_date','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','purchase_date_time',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','serial_number',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','transaction_date_time',null,'2',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation','units_produced',null,'3',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('depreciation','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('depreciation','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('depreciation','supervisor','lookup');
delete from $javascript_folders where folder = 'depreciation';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('depreciation','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('depreciation','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('depreciation','supervisor','detail');
delete from $folder where folder = 'depreciation_method';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('depreciation_method','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'depreciation_method';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','depreciation_method','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','depreciation_method','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_method','text','25',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'depreciation_method';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('depreciation_method','depreciation_method','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('depreciation_method','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('depreciation_method','supervisor','lookup');
delete from $javascript_folders where folder = 'depreciation_method';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','detail');
delete from $folder where folder = 'element';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('element','prompt','5','ledger',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'element';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('subclassification','element','null',null,null,null,null,'y',null,null,null,null,null,null);
delete from $attribute where attribute = 'element';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('element','text','20',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'accumulate_debit_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('accumulate_debit_yn','text','1',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'element';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('element','accumulate_debit_yn',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('element','element','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('element','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('element','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('element','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('element','supervisor','update');
delete from $javascript_folders where folder = 'element';
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('element','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('element','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('element','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('element','supervisor','detail');
delete from $folder where folder = 'employee';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('employee','prompt','5','entity',null,'n',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'employee';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee','federal_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee','state_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_day','employee','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_period','employee','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'hourly_wage';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('hourly_wage','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'period_salary';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('period_salary','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'commission_sum_extension_percent';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('commission_sum_extension_percent','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'gross_pay_year_to_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('gross_pay_year_to_date','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'net_pay_year_to_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('net_pay_year_to_date','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_marital_status';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_marital_status','text','25',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_withholding_allowances';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_withholding_allowances','integer','2',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_withholding_additional_period_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_withholding_additional_period_amount','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_marital_status';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_marital_status','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_withholding_allowances';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_withholding_allowances','integer','2',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_itemized_deduction_allowances';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_itemized_deduction_allowances','integer','2',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'retirement_contribution_plan_employer_period_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retirement_contribution_plan_employer_period_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'retirement_contribution_plan_employee_period_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retirement_contribution_plan_employee_period_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'health_insurance_employer_period_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('health_insurance_employer_period_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'health_insurance_employee_period_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('health_insurance_employee_period_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'union_dues_period_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('union_dues_period_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'terminated_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('terminated_date','date','10',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'employee';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','commission_sum_extension_percent',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','federal_marital_status',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','federal_withholding_additional_period_amount',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','federal_withholding_allowances',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','gross_pay_year_to_date',null,'4',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','health_insurance_employee_period_amount',null,'15',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','health_insurance_employer_period_amount',null,'14',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','hourly_wage',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','net_pay_year_to_date',null,'5',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','period_salary',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','retirement_contribution_plan_employee_period_amount',null,'13',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','retirement_contribution_plan_employer_period_amount',null,'12',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','state_itemized_deduction_allowances',null,'11',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','state_marital_status',null,'9',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','state_withholding_allowances',null,'10',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','terminated_date',null,'17',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee','union_dues_period_amount',null,'16',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('employee','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('employee','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('employee','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('employee','supervisor','update');
delete from $javascript_folders where folder = 'employee';
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('employee','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('employee','bookkeeper','delete_isa_only');
insert into $role_operation (folder,role,operation) values ('employee','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('employee','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('employee','supervisor','delete_isa_only');
insert into $role_operation (folder,role,operation) values ('employee','supervisor','detail');
delete from $folder where folder = 'employee_work_day';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('employee_work_day','prompt','1','payroll',null,null,null,null,null,null,null,'post_change_employee_work_day',null,null,null,null);
delete from $relation where folder = 'employee_work_day';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_day','employee','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'begin_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date_time','current_date_time','16',null,'Format: yyyy-mm-dd hh:mm',null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date_time','date_time','16',null,'Format: yyyy-mm-dd hh:mm',null,null,null,null,null);
delete from $attribute where attribute = 'employee_work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('employee_work_hours','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'overtime_work_day_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('overtime_work_day_yn','text','1',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'insert_timestamp';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('insert_timestamp','date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'update_timestamp';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('update_timestamp','date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'employee_work_day';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','begin_work_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','employee_work_hours',null,'2',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','end_work_date_time',null,'1',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','insert_timestamp',null,'5',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','overtime_work_day_yn',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_day','update_timestamp',null,'6',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('employee_work_day','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('employee_work_day','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('employee_work_day','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('employee_work_day','supervisor','update');
delete from $javascript_folders where folder = 'employee_work_day';
insert into $subschemas (subschema) values ('payroll');
insert into $role_operation (folder,role,operation) values ('employee_work_day','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('employee_work_day','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('employee_work_day','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('employee_work_day','supervisor','detail');
delete from $folder where folder = 'employee_work_period';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('employee_work_period','prompt','5','payroll',null,'y',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'employee_work_period';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_period','employee','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_period','payroll_posting','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_period','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_year';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_year','integer','4',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_period_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_period_number','integer','2',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'begin_work_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'regular_work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('regular_work_hours','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'overtime_work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('overtime_work_hours','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'gross_pay';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('gross_pay','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'net_pay';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('net_pay','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_tax_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'commission_sum_extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('commission_sum_extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_tax_withholding_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_tax_withholding_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'state_tax_withholding_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_tax_withholding_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'social_security_employee_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('social_security_employee_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'social_security_employer_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('social_security_employer_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'medicare_employee_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('medicare_employee_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'medicare_employer_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('medicare_employer_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'retirement_contribution_plan_employee_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retirement_contribution_plan_employee_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'retirement_contribution_plan_employer_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retirement_contribution_plan_employer_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'health_insurance_employee_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('health_insurance_employee_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'health_insurance_employer_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('health_insurance_employer_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'federal_unemployment_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_unemployment_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'state_unemployment_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_unemployment_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'union_dues_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('union_dues_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'employee_work_period';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','begin_work_date',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','commission_sum_extension',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','end_work_date',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','federal_tax_withholding_amount',null,'9',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','federal_unemployment_tax_amount',null,'19',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','gross_pay',null,'5',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','health_insurance_employee_amount',null,'17',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','health_insurance_employer_amount',null,'18',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','medicare_employee_tax_amount',null,'13',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','medicare_employer_tax_amount',null,'14',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','net_pay',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','overtime_work_hours',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','payroll_period_number','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','payroll_tax_amount',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','payroll_year','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','regular_work_hours',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','retirement_contribution_plan_employee_amount',null,'15',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','retirement_contribution_plan_employer_amount',null,'16',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','social_security_employee_tax_amount',null,'11',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','social_security_employer_tax_amount',null,'12',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','state_tax_withholding_amount',null,'10',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','state_unemployment_tax_amount',null,'20',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','transaction_date_time',null,'22',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('employee_work_period','union_dues_amount',null,'21',null,'y',null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('employee_work_period','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('employee_work_period','supervisor','lookup');
delete from $javascript_folders where folder = 'employee_work_period';
insert into $subschemas (subschema) values ('payroll');
insert into $role_operation (folder,role,operation) values ('employee_work_period','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('employee_work_period','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('employee_work_period','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('employee_work_period','supervisor','detail');
delete from $folder where folder = 'entity';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('entity','prompt','5','entity',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'entity';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('entity','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('liability_account_entity','entity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','entity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('transaction','entity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','20',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'city';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('city','text','20',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_code';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_code','text','2',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'zip_code';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('zip_code','text','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'phone_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('phone_number','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cell_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cell_number','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'email_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('email_address','text','50',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'entity';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','cell_number',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','city',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','email_address',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','login_name',null,'1',null,null,'y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','phone_number',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','state_code',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('entity','zip_code',null,'4',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('entity','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('entity','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('entity','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('entity','supervisor','update');
delete from $javascript_folders where folder = 'entity';
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('entity','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('entity','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('entity','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('entity','supervisor','detail');
delete from $folder where folder = 'federal_income_tax_withholding';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('federal_income_tax_withholding','prompt','10','tax',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'federal_income_tax_withholding';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('federal_income_tax_withholding','federal_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_marital_status';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_marital_status','text','25',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'income_over';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('income_over','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'income_not_over';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('income_not_over','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_fixed_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_fixed_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_percentage_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_percentage_amount','float','5','1','0.0 < TPA < 100.0',null,null,null,null,null);
delete from $folder_attribute where folder = 'federal_income_tax_withholding';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('federal_income_tax_withholding','federal_marital_status','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('federal_income_tax_withholding','income_not_over',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('federal_income_tax_withholding','income_over','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('federal_income_tax_withholding','tax_fixed_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('federal_income_tax_withholding','tax_percentage_amount',null,'3',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('federal_income_tax_withholding','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('federal_income_tax_withholding','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('federal_income_tax_withholding','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('federal_income_tax_withholding','supervisor','update');
delete from $javascript_folders where folder = 'federal_income_tax_withholding';
insert into $subschemas (subschema) values ('tax');
insert into $role_operation (folder,role,operation) values ('federal_income_tax_withholding','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('federal_income_tax_withholding','supervisor','delete');
delete from $folder where folder = 'federal_marital_status';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('federal_marital_status','table','3','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'federal_marital_status';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee','federal_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('federal_income_tax_withholding','federal_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_marital_status';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_marital_status','text','25',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'federal_marital_status';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('federal_marital_status','federal_marital_status','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('federal_marital_status','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('federal_marital_status','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('federal_marital_status','supervisor','update');
delete from $javascript_folders where folder = 'federal_marital_status';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('federal_marital_status','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('federal_marital_status','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('federal_marital_status','supervisor','detail');
delete from $folder where folder = 'fixed_asset';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('fixed_asset','prompt','5','purchase',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'fixed_asset';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','fixed_asset','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','fixed_asset','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_asset';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset','account',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset','asset_name','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('fixed_asset','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_asset','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset','supervisor','update');
delete from $javascript_folders where folder = 'fixed_asset';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_asset','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset','supervisor','detail');
delete from $folder where folder = 'fixed_asset_purchase';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('fixed_asset_purchase','prompt','5','purchase',null,null,null,null,null,null,null,'post_change_fixed_asset_purchase',null,null,null,null);
delete from $relation where folder = 'fixed_asset_purchase';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','depreciation_method','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','fixed_asset','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','purchase_order','null','4',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','fixed_asset_purchase','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'serial_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('serial_number','text','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_useful_life_years';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_useful_life_years','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_useful_life_units';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_useful_life_units','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_residual_value';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_residual_value','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'declining_balance_n';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('declining_balance_n','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_method','text','25',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'accumulated_depreciation';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('accumulated_depreciation','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'disposal_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('disposal_date','date','10',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_asset_purchase';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','accumulated_depreciation',null,'7',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','asset_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','declining_balance_n',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','depreciation_method',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','disposal_date',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','estimated_residual_value',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','estimated_useful_life_units',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','estimated_useful_life_years',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','extension',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','purchase_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','serial_number','5',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_asset_purchase','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_asset_purchase','supervisor','update');
delete from $javascript_folders where folder = 'fixed_asset_purchase';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','detail');
delete from $folder where folder = 'fixed_service';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('fixed_service','prompt','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'fixed_service';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service','fixed_service_category','null',null,null,null,null,null,null,null,null,'y',null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service_sale','fixed_service','null',null,null,null,null,null,null,null,null,null,'y',null);
delete from $attribute where attribute = 'service_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'retail_price';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retail_price','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'service_category';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_category','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_service';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service','account',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service','retail_price',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service','service_category',null,'1',null,null,null,null,null,null,'y');
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service','service_name','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('fixed_service','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_service','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service','supervisor','update');
delete from $javascript_folders where folder = 'fixed_service';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('fixed_service','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('fixed_service','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_service','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_service','supervisor','detail');
delete from $folder where folder = 'fixed_service_category';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('fixed_service_category','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'fixed_service_category';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service','fixed_service_category','null',null,null,null,null,null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'service_category';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_category','text','30',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_service_category';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_category','service_category','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('fixed_service_category','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service_category','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_service_category','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service_category','supervisor','update');
delete from $javascript_folders where folder = 'fixed_service_category';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('fixed_service_category','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_service_category','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_service_category','supervisor','detail');
delete from $folder where folder = 'fixed_service_sale';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('fixed_service_sale','prompt','5','sale',null,'y',null,null,null,null,null,'post_change_fixed_service_sale',null,null,null,null);
delete from $relation where folder = 'fixed_service_sale';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service_sale','customer_sale','null','2',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service_sale','fixed_service','null',null,null,null,null,null,null,null,null,null,'y',null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service_work','fixed_service_sale','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'service_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'retail_price';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retail_price','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'discount_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('discount_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('work_hours','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_service_sale';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','discount_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','extension',null,'4',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','retail_price',null,'1',null,null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','sale_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','service_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_sale','work_hours',null,'3',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('fixed_service_sale','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service_sale','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_service_sale','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service_sale','supervisor','update');
delete from $javascript_folders where folder = 'fixed_service_sale';
insert into $subschemas (subschema) values ('sale');
insert into $role_operation (folder,role,operation) values ('fixed_service_sale','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('fixed_service_sale','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_service_sale','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_service_sale','supervisor','detail');
delete from $folder where folder = 'fixed_service_work';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('fixed_service_work','prompt','1','sale',null,null,null,null,null,null,null,'post_change_fixed_service_work',null,null,null,null);
delete from $relation where folder = 'fixed_service_work';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service_work','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_service_work','fixed_service_sale','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'service_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'begin_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date_time','current_date_time','16',null,'Format: yyyy-mm-dd hh:mm',null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date_time','date_time','16',null,'Format: yyyy-mm-dd hh:mm',null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','20',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('work_hours','float','10','4',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'fixed_service_work';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','begin_work_date_time','5',null,null,'n',null,null,'n',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','end_work_date_time',null,'1',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','login_name',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','sale_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','service_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('fixed_service_work','work_hours',null,'3',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('fixed_service_work','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service_work','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('fixed_service_work','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('fixed_service_work','supervisor','update');
delete from $javascript_folders where folder = 'fixed_service_work';
insert into $subschemas (subschema) values ('sale');
insert into $role_operation (folder,role,operation) values ('fixed_service_work','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('fixed_service_work','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('fixed_service_work','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('fixed_service_work','supervisor','detail');
delete from $folder where folder = 'hourly_service';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('hourly_service','prompt','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'hourly_service';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service','hourly_service_category','null',null,null,null,null,null,null,null,null,'y',null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service_sale','hourly_service','null',null,null,null,null,null,null,null,null,null,'y',null);
delete from $attribute where attribute = 'service_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'hourly_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('hourly_rate','float','7','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'service_category';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_category','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'hourly_service';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service','account',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service','hourly_rate',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service','service_category',null,'1',null,null,null,null,null,null,'y');
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service','service_name','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('hourly_service','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('hourly_service','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service','supervisor','update');
delete from $javascript_folders where folder = 'hourly_service';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('hourly_service','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('hourly_service','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('hourly_service','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('hourly_service','supervisor','detail');
delete from $folder where folder = 'hourly_service_category';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('hourly_service_category','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'hourly_service_category';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service','hourly_service_category','null',null,null,null,null,null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'service_category';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_category','text','30',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'hourly_service_category';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_category','service_category','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('hourly_service_category','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service_category','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('hourly_service_category','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service_category','supervisor','update');
delete from $javascript_folders where folder = 'hourly_service_category';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('hourly_service_category','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('hourly_service_category','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('hourly_service_category','supervisor','detail');
delete from $folder where folder = 'hourly_service_sale';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('hourly_service_sale','prompt','5','sale',null,'y',null,null,null,null,null,'post_change_hourly_service_sale',null,null,null,null);
delete from $relation where folder = 'hourly_service_sale';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service_sale','customer_sale','null','5',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service_sale','hourly_service','null',null,null,null,null,null,null,null,null,null,'y',null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service_work','hourly_service_sale','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'service_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('description','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_hours','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'hourly_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('hourly_rate','float','7','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('work_hours','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'hourly_service_sale';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','description','5',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','estimated_hours',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','extension',null,'5',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','hourly_rate',null,'3',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','sale_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','service_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_sale','work_hours',null,'3',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('hourly_service_sale','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service_sale','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('hourly_service_sale','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service_sale','supervisor','update');
delete from $javascript_folders where folder = 'hourly_service_sale';
insert into $subschemas (subschema) values ('sale');
insert into $role_operation (folder,role,operation) values ('hourly_service_sale','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('hourly_service_sale','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('hourly_service_sale','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('hourly_service_sale','supervisor','detail');
delete from $folder where folder = 'hourly_service_work';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('hourly_service_work','prompt','1','sale',null,null,null,null,null,null,null,'post_change_hourly_service_work',null,null,null,null);
delete from $relation where folder = 'hourly_service_work';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service_work','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('hourly_service_work','hourly_service_sale','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'service_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('description','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'begin_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date_time','current_date_time','16',null,'Format: yyyy-mm-dd hh:mm',null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date_time','date_time','16',null,'Format: yyyy-mm-dd hh:mm',null,null,null,null,null);
delete from $attribute where attribute = 'login_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('login_name','text','20',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('work_hours','float','10','4',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'hourly_service_work';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','begin_work_date_time','6',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','description','5',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','end_work_date_time',null,'1',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','login_name',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','sale_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','service_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('hourly_service_work','work_hours',null,'3',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('hourly_service_work','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service_work','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('hourly_service_work','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('hourly_service_work','supervisor','update');
delete from $javascript_folders where folder = 'hourly_service_work';
insert into $subschemas (subschema) values ('sale');
insert into $role_operation (folder,role,operation) values ('hourly_service_work','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('hourly_service_work','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('hourly_service_work','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('hourly_service_work','supervisor','detail');
delete from $folder where folder = 'inventory';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('inventory','prompt','5','purchase',null,null,'populate_inventory',null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'inventory';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','account','cost_of_goods_sold_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','account','inventory_account',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','inventory_category','null',null,null,null,null,null,null,null,null,'y',null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_purchase','inventory','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_sale','inventory','null',null,null,null,null,null,null,null,null,null,'y',null);
delete from $attribute where attribute = 'inventory_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'inventory_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_of_goods_sold_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_of_goods_sold_account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'retail_price';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retail_price','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'inventory_category';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_category','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'reorder_quantity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('reorder_quantity','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'quantity_on_hand';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('quantity_on_hand','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'average_unit_cost';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('average_unit_cost','float','12','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'total_cost_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('total_cost_balance','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'inventory';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','average_unit_cost',null,'7',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','cost_of_goods_sold_account',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','inventory_account',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','inventory_category',null,'4',null,null,null,null,null,null,'y');
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','inventory_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','quantity_on_hand',null,'6',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','reorder_quantity',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','retail_price',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory','total_cost_balance',null,'8',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('inventory','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('inventory','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('inventory','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('inventory','supervisor','update');
delete from $javascript_folders where folder = 'inventory';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('inventory','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('inventory','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('inventory','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('inventory','supervisor','detail');
delete from $folder where folder = 'inventory_category';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('inventory_category','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'inventory_category';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory','inventory_category','null',null,null,null,null,null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'inventory_category';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_category','text','30',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'inventory_category';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_category','inventory_category','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('inventory_category','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('inventory_category','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('inventory_category','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('inventory_category','supervisor','update');
delete from $javascript_folders where folder = 'inventory_category';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('inventory_category','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('inventory_category','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('inventory_category','supervisor','detail');
delete from $folder where folder = 'inventory_cost_method';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('inventory_cost_method','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'inventory_cost_method';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','inventory_cost_method','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'inventory_cost_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_cost_method','text','15',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'inventory_cost_method';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_cost_method','inventory_cost_method','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('inventory_cost_method','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('inventory_cost_method','bookkeeper','update');
delete from $javascript_folders where folder = 'inventory_cost_method';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('inventory_cost_method','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('inventory_cost_method','bookkeeper','detail');
delete from $folder where folder = 'inventory_purchase';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('inventory_purchase','prompt','5','purchase',null,null,null,null,null,null,null,'post_change_inventory_purchase',null,null,null,null);
delete from $relation where folder = 'inventory_purchase';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_purchase','inventory','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_purchase','purchase_order','null','1',null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'inventory_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'ordered_quantity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('ordered_quantity','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'arrived_quantity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('arrived_quantity','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'missing_quantity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('missing_quantity','integer','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'quantity_on_hand';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('quantity_on_hand','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'capitalized_unit_cost';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('capitalized_unit_cost','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'unit_cost';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('unit_cost','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'average_unit_cost';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('average_unit_cost','float','12','4',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'inventory_purchase';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','arrived_quantity',null,'2',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','average_unit_cost',null,'9',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','capitalized_unit_cost',null,'5',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','extension',null,'6',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','inventory_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','missing_quantity',null,'3',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','ordered_quantity',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','purchase_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','quantity_on_hand',null,'4',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_purchase','unit_cost',null,'5',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('inventory_purchase','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('inventory_purchase','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('inventory_purchase','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('inventory_purchase','supervisor','update');
delete from $javascript_folders where folder = 'inventory_purchase';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('inventory_purchase','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('inventory_purchase','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('inventory_purchase','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('inventory_purchase','supervisor','detail');
delete from $folder where folder = 'inventory_sale';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('inventory_sale','prompt','10','sale',null,null,null,null,null,null,null,'post_change_inventory_sale',null,null,null,null);
delete from $relation where folder = 'inventory_sale';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_sale','customer_sale','null','1',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_sale','inventory','null',null,null,null,null,null,null,null,null,null,'y',null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'sale_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sale_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'inventory_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'quantity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('quantity','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'retail_price';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retail_price','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'discount_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('discount_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'cost_of_goods_sold';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('cost_of_goods_sold','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'inventory_sale';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','cost_of_goods_sold',null,'5','n','y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','discount_amount',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','extension',null,'4','n','y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','inventory_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','quantity',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','retail_price',null,'2',null,null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','sale_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('inventory_sale','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('inventory_sale','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('inventory_sale','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('inventory_sale','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('inventory_sale','supervisor','update');
delete from $javascript_folders where folder = 'inventory_sale';
insert into $subschemas (subschema) values ('sale');
insert into $role_operation (folder,role,operation) values ('inventory_sale','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('inventory_sale','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('inventory_sale','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('inventory_sale','supervisor','detail');
delete from $folder where folder = 'journal_ledger';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('journal_ledger','prompt','10','ledger',null,null,null,null,null,null,'post_change_journal_ledger( ''\$state'' )','post_change_journal_ledger',null,null,null,null);
delete from $relation where folder = 'journal_ledger';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','account','null',null,null,null,null,'y',null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','transaction','null','1',null,null,null,'y',null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_count';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_count','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'previous_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('previous_balance','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'debit_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('debit_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'credit_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('credit_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('balance','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'journal_ledger';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','account','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','balance',null,'10','y','y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','credit_amount',null,'9',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','debit_amount',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','previous_balance',null,'7','y','y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','transaction_count',null,'1',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('journal_ledger','transaction_date_time','3',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('journal_ledger','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('journal_ledger','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('journal_ledger','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('journal_ledger','supervisor','update');
delete from $javascript_folders where folder = 'journal_ledger';
insert into $javascript_folders (javascript_filename,folder) values ('post_change_journal_ledger.js','journal_ledger');
delete from $javascript_files where javascript_filename = 'post_change_journal_ledger.js';
insert into $javascript_files (javascript_filename) values ('post_change_journal_ledger.js');
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('journal_ledger','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('journal_ledger','supervisor','detail');
delete from $folder where folder = 'liability_account_entity';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('liability_account_entity','prompt','5','entity',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'liability_account_entity';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('liability_account_entity','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('liability_account_entity','entity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'liability_account_entity';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('liability_account_entity','account','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('liability_account_entity','full_name',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('liability_account_entity','street_address',null,'2',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('liability_account_entity','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('liability_account_entity','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('liability_account_entity','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('liability_account_entity','supervisor','update');
delete from $javascript_folders where folder = 'liability_account_entity';
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('liability_account_entity','supervisor','detail');
delete from $folder where folder = 'payroll_pay_period';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('payroll_pay_period','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'payroll_pay_period';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','payroll_pay_period','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_pay_period';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_pay_period','text','15',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'payroll_pay_period';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_pay_period','payroll_pay_period','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('payroll_pay_period','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('payroll_pay_period','supervisor','update');
delete from $javascript_folders where folder = 'payroll_pay_period';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('payroll_pay_period','supervisor','delete');
delete from $folder where folder = 'payroll_posting';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('payroll_posting','prompt','5','payroll',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'payroll_posting';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_period','payroll_posting','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_year';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_year','integer','4',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_period_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_period_number','integer','2',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'begin_work_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('begin_work_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'end_work_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('end_work_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'regular_work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('regular_work_hours','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'overtime_work_hours';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('overtime_work_hours','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'gross_pay';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('gross_pay','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'net_pay';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('net_pay','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_tax_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'commission_sum_extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('commission_sum_extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_tax_withholding_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_tax_withholding_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'state_tax_withholding_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_tax_withholding_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'social_security_employee_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('social_security_employee_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'social_security_employer_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('social_security_employer_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'medicare_employee_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('medicare_employee_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'medicare_employer_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('medicare_employer_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'retirement_contribution_plan_employee_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retirement_contribution_plan_employee_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'retirement_contribution_plan_employer_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('retirement_contribution_plan_employer_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'health_insurance_employee_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('health_insurance_employee_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'health_insurance_employer_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('health_insurance_employer_amount','integer','5',null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'federal_unemployment_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_unemployment_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'state_unemployment_tax_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_unemployment_tax_amount','float','10','2',null,null,null,'y',null,null);
delete from $attribute where attribute = 'union_dues_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('union_dues_amount','integer','5',null,null,null,null,'y',null,null);
delete from $folder_attribute where folder = 'payroll_posting';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','begin_work_date',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','commission_sum_extension',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','end_work_date',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','federal_tax_withholding_amount',null,'9',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','federal_unemployment_tax_amount',null,'19',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','gross_pay',null,'5',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','health_insurance_employee_amount',null,'17',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','health_insurance_employer_amount',null,'18',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','medicare_employee_tax_amount',null,'13',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','medicare_employer_tax_amount',null,'14',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','net_pay',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','overtime_work_hours',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','payroll_period_number','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','payroll_tax_amount',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','payroll_year','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','regular_work_hours',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','retirement_contribution_plan_employee_amount',null,'15',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','retirement_contribution_plan_employer_amount',null,'16',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','social_security_employee_tax_amount',null,'11',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','social_security_employer_tax_amount',null,'12',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','state_tax_withholding_amount',null,'10',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','state_unemployment_tax_amount',null,'20',null,'y',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('payroll_posting','union_dues_amount',null,'21',null,'y',null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('payroll_posting','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('payroll_posting','supervisor','lookup');
delete from $javascript_folders where folder = 'payroll_posting';
insert into $subschemas (subschema) values ('payroll');
insert into $role_operation (folder,role,operation) values ('payroll_posting','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('payroll_posting','supervisor','detail');
delete from $folder where folder = 'prior_fixed_asset';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('prior_fixed_asset','prompt','5','purchase',null,'y',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'prior_fixed_asset';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','depreciation','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','depreciation_method','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','fixed_asset','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'serial_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('serial_number','text','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'recorded_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('recorded_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_useful_life_years';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_useful_life_years','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_useful_life_units';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_useful_life_units','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'estimated_residual_value';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('estimated_residual_value','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'declining_balance_n';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('declining_balance_n','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_method','text','25',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'disposal_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('disposal_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'accumulated_depreciation';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('accumulated_depreciation','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'prior_fixed_asset';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','accumulated_depreciation',null,'11',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','asset_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','declining_balance_n',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','depreciation_method',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','disposal_date',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','estimated_residual_value',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','estimated_useful_life_units',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','estimated_useful_life_years',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','extension',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','full_name',null,'8',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','recorded_date',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','serial_number','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','street_address',null,'9',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset','transaction_date_time',null,'10',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('prior_fixed_asset','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('prior_fixed_asset','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('prior_fixed_asset','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('prior_fixed_asset','supervisor','update');
delete from $javascript_folders where folder = 'prior_fixed_asset';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('prior_fixed_asset','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('prior_fixed_asset','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('prior_fixed_asset','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('prior_fixed_asset','supervisor','detail');
delete from $folder where folder = 'prior_fixed_asset_depreciation';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('prior_fixed_asset_depreciation','table','5','purchase',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'prior_fixed_asset_depreciation';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset_depreciation','self','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset_depreciation','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'asset_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('asset_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'serial_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('serial_number','text','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'depreciation_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('depreciation_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'prior_fixed_asset_depreciation';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset_depreciation','asset_name','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset_depreciation','depreciation_amount',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset_depreciation','depreciation_date','5',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset_depreciation','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset_depreciation','serial_number','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset_depreciation','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('prior_fixed_asset_depreciation','transaction_date_time',null,'1',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('prior_fixed_asset_depreciation','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('prior_fixed_asset_depreciation','supervisor','lookup');
delete from $javascript_folders where folder = 'prior_fixed_asset_depreciation';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('prior_fixed_asset_depreciation','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('prior_fixed_asset_depreciation','supervisor','detail');
delete from $folder where folder = 'purchase_order';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('purchase_order','prompt','1','purchase',null,'y',null,'Restriction warning: Inventory purchase orders should not be comingled with other purchase orders. Sales tax and freight need to be capitalized into inventory, not expensed.',null,null,null,'post_change_purchase_order',null,null,null,null);
delete from $relation where folder = 'purchase_order';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('purchase_order','title_passage_rule','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('purchase_order','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('purchase_order','vendor','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('fixed_asset_purchase','purchase_order','null','4',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('inventory_purchase','purchase_order','null','1',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('service_purchase','purchase_order','null','3',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('supply_purchase','purchase_order','null','2',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor_payment','purchase_order','null','9',null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'amount_due';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('amount_due','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'sum_extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sum_extension','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'sales_tax';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sales_tax','float','7','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'freight_in';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('freight_in','float','6','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'title_passage_rule';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('title_passage_rule','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'shipped_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('shipped_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'arrived_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('arrived_date_time','date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'purchase_order';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','amount_due',null,'8',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','arrived_date_time',null,'70',null,'n',null,null,'n',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','freight_in',null,'30',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','purchase_amount',null,'40',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','purchase_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','sales_tax',null,'20',null,'n',null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','shipped_date',null,'60',null,'y',null,null,'n',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','sum_extension',null,'10',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','title_passage_rule',null,'50',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('purchase_order','transaction_date_time',null,'80',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('purchase_order','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('purchase_order','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('purchase_order','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('purchase_order','supervisor','update');
delete from $javascript_folders where folder = 'purchase_order';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('purchase_order','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('purchase_order','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('purchase_order','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('purchase_order','supervisor','detail');
delete from $folder where folder = 'receipt_upload';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('receipt_upload','prompt','5','ledger',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'receipt_upload';
delete from $attribute where attribute = 'receipt_upload_filename';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('receipt_upload_filename','http_filename','80',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'upload_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('upload_date','current_date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'posted_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('posted_yn','text','1',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'receipt_upload';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('receipt_upload','posted_yn',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('receipt_upload','receipt_upload_filename','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('receipt_upload','upload_date',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('receipt_upload','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('receipt_upload','supervisor','update');
delete from $javascript_folders where folder = 'receipt_upload';
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('receipt_upload','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('receipt_upload','supervisor','delete');
delete from $folder where folder = 'reoccurring_transaction';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('reoccurring_transaction','prompt','5','ledger',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'reoccurring_transaction';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','account','credit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','account','debit_account',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('reoccurring_transaction','entity','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'debit_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('debit_account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'credit_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('credit_account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_amount','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'reoccurring_transaction';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('reoccurring_transaction','credit_account',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('reoccurring_transaction','debit_account',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('reoccurring_transaction','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('reoccurring_transaction','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('reoccurring_transaction','transaction_amount',null,'3',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('reoccurring_transaction','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('reoccurring_transaction','supervisor','update');
delete from $javascript_folders where folder = 'reoccurring_transaction';
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('reoccurring_transaction','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('reoccurring_transaction','supervisor','detail');
delete from $folder where folder = 'self';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('self','table','1','entity',null,null,null,null,null,null,null,'post_change_self',null,null,null,null);
delete from $relation where folder = 'self';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','day','payroll_beginning_day',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','inventory_cost_method','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('self','payroll_pay_period','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset_depreciation','self','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'inventory_cost_method';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('inventory_cost_method','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_pay_period';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_pay_period','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payroll_beginning_day';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payroll_beginning_day','text','9',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'social_security_combined_tax_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('social_security_combined_tax_rate','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'social_security_payroll_ceiling';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('social_security_payroll_ceiling','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'medicare_combined_tax_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('medicare_combined_tax_rate','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'medicare_additional_withholding_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('medicare_additional_withholding_rate','float','5','3',null,null,null,null,null,null);
delete from $attribute where attribute = 'medicare_additional_gross_pay_floor';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('medicare_additional_gross_pay_floor','integer','7',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_withholding_allowance_period_value';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_withholding_allowance_period_value','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_nonresident_withholding_income_premium';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_nonresident_withholding_income_premium','float','7','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'state_withholding_allowance_period_value';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_withholding_allowance_period_value',null,'7','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'state_itemized_allowance_period_value';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_itemized_allowance_period_value','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_unemployment_wage_base';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_unemployment_wage_base','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_unemployment_tax_standard_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_unemployment_tax_standard_rate','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_unemployment_threshold_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_unemployment_threshold_rate','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'federal_unemployment_tax_minimum_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('federal_unemployment_tax_minimum_rate','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'state_unemployment_wage_base';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_unemployment_wage_base','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_unemployment_tax_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_unemployment_tax_rate','float','10','4',null,null,null,null,null,null);
delete from $attribute where attribute = 'state_sales_tax_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_sales_tax_rate','float','6','4',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'self';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','federal_nonresident_withholding_income_premium',null,'10',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','federal_unemployment_tax_minimum_rate',null,'16',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','federal_unemployment_tax_standard_rate',null,'14',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','federal_unemployment_threshold_rate',null,'15',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','federal_unemployment_wage_base',null,'13',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','federal_withholding_allowance_period_value',null,'9',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','inventory_cost_method',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','medicare_additional_gross_pay_floor',null,'8',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','medicare_additional_withholding_rate',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','medicare_combined_tax_rate',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','payroll_beginning_day',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','payroll_pay_period',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','social_security_combined_tax_rate',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','social_security_payroll_ceiling',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','state_itemized_allowance_period_value',null,'12',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','state_sales_tax_rate',null,'19',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','state_unemployment_tax_rate',null,'18',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','state_unemployment_wage_base',null,'17',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','state_withholding_allowance_period_value',null,'11',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('self','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('self','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('self','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('self','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('self','supervisor','update');
delete from $javascript_folders where folder = 'self';
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('self','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('self','bookkeeper','detail');
delete from $folder where folder = 'service_purchase';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('service_purchase','prompt','5','purchase',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'service_purchase';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('service_purchase','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('service_purchase','purchase_order','null','3',null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'service_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('service_description','notepad','1024',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'service_purchase';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('service_purchase','account','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('service_purchase','extension',null,'2',null,null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('service_purchase','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('service_purchase','purchase_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('service_purchase','service_description',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('service_purchase','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('service_purchase','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('service_purchase','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('service_purchase','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('service_purchase','supervisor','update');
delete from $javascript_folders where folder = 'service_purchase';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('service_purchase','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('service_purchase','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('service_purchase','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('service_purchase','supervisor','detail');
delete from $folder where folder = 'state_income_tax_withholding';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('state_income_tax_withholding','prompt','10','tax',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'state_income_tax_withholding';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('state_income_tax_withholding','state_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_marital_status';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_marital_status','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'income_over';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('income_over','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'income_not_over';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('income_not_over','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_fixed_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_fixed_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_percentage_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_percentage_amount','float','5','1','0.0 < TPA < 100.0',null,null,null,null,null);
delete from $folder_attribute where folder = 'state_income_tax_withholding';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_income_tax_withholding','income_not_over',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_income_tax_withholding','income_over','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_income_tax_withholding','state_marital_status','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_income_tax_withholding','tax_fixed_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_income_tax_withholding','tax_percentage_amount',null,'3',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('state_income_tax_withholding','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('state_income_tax_withholding','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('state_income_tax_withholding','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('state_income_tax_withholding','supervisor','update');
delete from $javascript_folders where folder = 'state_income_tax_withholding';
insert into $subschemas (subschema) values ('tax');
insert into $role_operation (folder,role,operation) values ('state_income_tax_withholding','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('state_income_tax_withholding','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('state_income_tax_withholding','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('state_income_tax_withholding','supervisor','detail');
delete from $folder where folder = 'state_marital_status';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('state_marital_status','table','5','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'state_marital_status';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee','state_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('state_income_tax_withholding','state_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('state_standard_deduction_table','state_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_marital_status';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_marital_status','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'state_marital_status';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_marital_status','state_marital_status','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('state_marital_status','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('state_marital_status','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('state_marital_status','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('state_marital_status','supervisor','update');
delete from $javascript_folders where folder = 'state_marital_status';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('state_marital_status','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('state_marital_status','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('state_marital_status','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('state_marital_status','supervisor','detail');
delete from $folder where folder = 'state_standard_deduction_table';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('state_standard_deduction_table','prompt','10','tax',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'state_standard_deduction_table';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('state_standard_deduction_table','state_marital_status','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_marital_status';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_marital_status','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_withholding_allowances';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_withholding_allowances','integer','2',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'state_standard_deduction_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('state_standard_deduction_amount','float','8','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'state_standard_deduction_table';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_standard_deduction_table','state_marital_status','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_standard_deduction_table','state_standard_deduction_amount',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('state_standard_deduction_table','state_withholding_allowances','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('state_standard_deduction_table','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('state_standard_deduction_table','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('state_standard_deduction_table','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('state_standard_deduction_table','supervisor','update');
delete from $javascript_folders where folder = 'state_standard_deduction_table';
insert into $subschemas (subschema) values ('tax');
insert into $role_operation (folder,role,operation) values ('state_standard_deduction_table','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('state_standard_deduction_table','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('state_standard_deduction_table','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('state_standard_deduction_table','supervisor','detail');
delete from $folder where folder = 'subclassification';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('subclassification','prompt','5','ledger',null,'y',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'subclassification';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('subclassification','element','null',null,null,null,null,'y',null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('account','subclassification','null',null,null,null,null,null,null,null,null,'y',null,null);
delete from $attribute where attribute = 'subclassification';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('subclassification','text','35',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'element';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('element','text','20',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'display_order';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('display_order','integer','3',null,null,null,null,null,null,'y');
delete from $folder_attribute where folder = 'subclassification';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subclassification','display_order',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subclassification','element',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('subclassification','subclassification','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('subclassification','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('subclassification','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('subclassification','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('subclassification','supervisor','update');
delete from $javascript_folders where folder = 'subclassification';
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('subclassification','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('subclassification','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('subclassification','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('subclassification','supervisor','detail');
delete from $folder where folder = 'supply';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('supply','prompt','5','purchase',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'supply';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('supply','account','null',null,null,null,null,null,null,null,'y',null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('supply_purchase','supply','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'supply_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('supply_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'supply';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply','account',null,'1',null,null,null,null,null,null,'y');
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply','supply_name','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('supply','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('supply','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('supply','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('supply','supervisor','update');
delete from $javascript_folders where folder = 'supply';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('supply','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('supply','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('supply','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('supply','supervisor','detail');
delete from $folder where folder = 'supply_purchase';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('supply_purchase','prompt','5','purchase',null,null,null,null,null,null,null,'post_change_supply_purchase',null,null,null,null);
delete from $relation where folder = 'supply_purchase';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('supply_purchase','purchase_order','null','2',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('supply_purchase','supply','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'supply_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('supply_name','text','30',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'quantity';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('quantity','integer','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'unit_cost';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('unit_cost','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'extension';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('extension','float','10','2',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'supply_purchase';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply_purchase','extension',null,'3',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply_purchase','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply_purchase','purchase_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply_purchase','quantity',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply_purchase','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply_purchase','supply_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('supply_purchase','unit_cost',null,'2',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('supply_purchase','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('supply_purchase','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('supply_purchase','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('supply_purchase','supervisor','update');
delete from $javascript_folders where folder = 'supply_purchase';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('supply_purchase','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('supply_purchase','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('supply_purchase','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('supply_purchase','supervisor','detail');
delete from $folder where folder = 'tax_form';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('tax_form','prompt','5','tax',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'tax_form';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('tax_form_line','tax_form','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_form';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_form','text','20',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'tax_form';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form','tax_form','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('tax_form','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('tax_form','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('tax_form','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('tax_form','supervisor','update');
delete from $javascript_folders where folder = 'tax_form';
insert into $subschemas (subschema) values ('tax');
insert into $role_operation (folder,role,operation) values ('tax_form','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('tax_form','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('tax_form','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('tax_form','supervisor','detail');
delete from $folder where folder = 'tax_form_line';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('tax_form_line','prompt','20','tax',null,null,'populate_tax_form_line',null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'tax_form_line';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('tax_form_line','tax_form','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('tax_form_line_account','tax_form_line','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_form';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_form','text','20',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_form_line';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_form_line','text','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_form_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_form_description','text','35',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'itemize_accounts_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('itemize_accounts_yn','text','1',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'tax_form_line';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form_line','itemize_accounts_yn',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form_line','tax_form','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form_line','tax_form_description',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form_line','tax_form_line','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('tax_form_line','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('tax_form_line','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('tax_form_line','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('tax_form_line','supervisor','update');
delete from $javascript_folders where folder = 'tax_form_line';
insert into $subschemas (subschema) values ('tax');
insert into $role_operation (folder,role,operation) values ('tax_form_line','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('tax_form_line','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('tax_form_line','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('tax_form_line','supervisor','detail');
delete from $folder where folder = 'tax_form_line_account';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('tax_form_line_account','prompt','5','tax',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'tax_form_line_account';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('tax_form_line_account','account','null','1',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('tax_form_line_account','tax_form_line','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_form';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_form','text','20',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'tax_form_line';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('tax_form_line','text','5',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account','text','60',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'tax_form_line_account';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form_line_account','account','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form_line_account','tax_form','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('tax_form_line_account','tax_form_line','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('tax_form_line_account','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('tax_form_line_account','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('tax_form_line_account','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('tax_form_line_account','supervisor','update');
delete from $javascript_folders where folder = 'tax_form_line_account';
insert into $subschemas (subschema) values ('tax');
insert into $role_operation (folder,role,operation) values ('tax_form_line_account','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('tax_form_line_account','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('tax_form_line_account','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('tax_form_line_account','supervisor','detail');
delete from $folder where folder = 'title_passage_rule';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('title_passage_rule','table','3','static',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'title_passage_rule';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('purchase_order','title_passage_rule','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'title_passage_rule';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('title_passage_rule','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'title_passage_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('title_passage_date','text','15',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'title_passage_rule';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('title_passage_rule','title_passage_date',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('title_passage_rule','title_passage_rule','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('title_passage_rule','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('title_passage_rule','bookkeeper','update');
delete from $javascript_folders where folder = 'title_passage_rule';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('title_passage_rule','bookkeeper','delete');
delete from $folder where folder = 'transaction';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('transaction','prompt','1','ledger',null,'y',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'transaction';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('transaction','entity','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_payment','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('customer_sale','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('depreciation','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('employee_work_period','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('journal_ledger','transaction','null','1',null,null,null,'y',null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('prior_fixed_asset_depreciation','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('purchase_order','transaction','null',null,null,null,'n',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor_payment','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'memo';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('memo','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'check_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('check_number','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'lock_transaction_yn';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('lock_transaction_yn','text','1',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'transaction';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','check_number',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','lock_transaction_yn',null,'4',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','memo',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','transaction_amount',null,'1',null,'y',null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('transaction','transaction_date_time','3',null,null,null,'y',null,null,null,null);
insert into $role_folder (folder,role,permission) values ('transaction','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('transaction','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('transaction','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('transaction','supervisor','update');
insert into $row_security_role_update (folder,attribute_not_null) values ('transaction','lock_transaction_yn');
delete from $javascript_folders where folder = 'transaction';
insert into $subschemas (subschema) values ('ledger');
insert into $role_operation (folder,role,operation) values ('transaction','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('transaction','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('transaction','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('transaction','supervisor','detail');
delete from $folder where folder = 'vendor';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('vendor','prompt','5','entity',null,'n',null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'vendor';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor','entity','null',null,null,null,'y',null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('purchase_order','vendor','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'vendor';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('vendor','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('vendor','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('vendor','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('vendor','supervisor','update');
delete from $javascript_folders where folder = 'vendor';
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('vendor','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('vendor','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('vendor','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('vendor','supervisor','detail');
delete from $folder where folder = 'vendor_payment';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('vendor_payment','prompt','5','purchase',null,'y',null,null,null,null,null,'post_change_vendor_payment',null,null,null,null);
delete from $relation where folder = 'vendor_payment';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor_payment','purchase_order','null','9',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,drop_down_multi_select_yn,automatic_preselection_yn,join_1tom_each_row_yn,omit_lookup_before_drop_down_yn,ajax_fill_drop_down_yn,copy_common_attributes_yn,hint_message) values ('vendor_payment','transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'purchase_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('purchase_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payment_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payment_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'payment_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('payment_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'check_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('check_number','integer','6',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'vendor_payment';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor_payment','check_number',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor_payment','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor_payment','payment_amount',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor_payment','payment_date_time','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor_payment','purchase_date_time','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor_payment','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('vendor_payment','transaction_date_time',null,'3',null,'y',null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('vendor_payment','bookkeeper','insert');
insert into $role_folder (folder,role,permission) values ('vendor_payment','bookkeeper','update');
insert into $role_folder (folder,role,permission) values ('vendor_payment','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('vendor_payment','supervisor','update');
delete from $javascript_folders where folder = 'vendor_payment';
insert into $subschemas (subschema) values ('purchase');
insert into $role_operation (folder,role,operation) values ('vendor_payment','bookkeeper','delete');
insert into $role_operation (folder,role,operation) values ('vendor_payment','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('vendor_payment','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('vendor_payment','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

table_name=`get_table_name $application account`
echo "create table $table_name (account char (60) not null,subclassification char (35),hard_coded_account_key char (40)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (account);" | sql.e '^' mysql
table_name=`get_table_name $application bank_upload`
echo "create table $table_name (bank_date date not null,bank_description char (140) not null,sequence_number integer,bank_amount double (10,2),bank_running_balance double (12,2),posted_yn char (1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (bank_date,bank_description);" | sql.e '^' mysql
table_name=`get_table_name $application contra_account`
echo "create table $table_name (account char (60) not null,contra_to_account char (60)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (account);" | sql.e '^' mysql
table_name=`get_table_name $application customer`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sales_tax_exempt_yn char (1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql
table_name=`get_table_name $application customer_payment`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sale_date_time datetime not null,payment_date_time datetime not null,payment_amount double (10,2),check_number integer,transaction_date_time datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,payment_date_time);" | sql.e '^' mysql
table_name=`get_table_name $application customer_sale`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sale_date_time datetime not null,shipping_revenue double (8,2),sum_extension double (10,2),sales_tax double (7,2),invoice_amount double (10,2),total_payment double (10,2),amount_due double (10,2),completed_date_time datetime,uncollectible_writeoff_date date,transaction_date_time datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time);" | sql.e '^' mysql
table_name=`get_table_name $application day`
echo "create table $table_name (day char (9) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (day);" | sql.e '^' mysql
table_name=`get_table_name $application depreciation`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,depreciation_date date not null,purchase_date_time datetime,asset_name char (30),transaction_date_time datetime,serial_number char (10),units_produced integer,depreciation_amount double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,depreciation_date);" | sql.e '^' mysql
table_name=`get_table_name $application depreciation_method`
echo "create table $table_name (depreciation_method char (25) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (depreciation_method);" | sql.e '^' mysql
table_name=`get_table_name $application element`
echo "create table $table_name (element char (20) not null,accumulate_debit_yn char (1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (element);" | sql.e '^' mysql
table_name=`get_table_name $application employee`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,hourly_wage double (10,2),period_salary double (10,2),commission_sum_extension_percent double (10,2),gross_pay_year_to_date double (10,2),net_pay_year_to_date double (10,2),federal_marital_status char (25),federal_withholding_allowances integer,federal_withholding_additional_period_amount integer,state_marital_status char (40),state_withholding_allowances integer,state_itemized_deduction_allowances integer,retirement_contribution_plan_employer_period_amount integer,retirement_contribution_plan_employee_period_amount integer,health_insurance_employer_period_amount integer,health_insurance_employee_period_amount integer,union_dues_period_amount integer,terminated_date date) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql
table_name=`get_table_name $application employee_work_day`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,begin_work_date_time datetime not null,end_work_date_time datetime,employee_work_hours double (6,2),overtime_work_day_yn char (1),insert_timestamp datetime,update_timestamp datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,begin_work_date_time);" | sql.e '^' mysql
table_name=`get_table_name $application employee_work_period`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,payroll_year integer not null,payroll_period_number integer not null,begin_work_date date,end_work_date date,regular_work_hours double (6,2),overtime_work_hours double (6,2),gross_pay double (10,2),net_pay double (10,2),payroll_tax_amount double (10,2),commission_sum_extension double (10,2),federal_tax_withholding_amount double (10,2),state_tax_withholding_amount double (10,2),social_security_employee_tax_amount double (10,2),social_security_employer_tax_amount double (10,2),medicare_employee_tax_amount double (10,2),medicare_employer_tax_amount double (10,2),retirement_contribution_plan_employee_amount integer,retirement_contribution_plan_employer_amount integer,health_insurance_employee_amount integer,health_insurance_employer_amount integer,federal_unemployment_tax_amount double (10,2),state_unemployment_tax_amount double (10,2),union_dues_amount integer,transaction_date_time datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,payroll_year,payroll_period_number);" | sql.e '^' mysql
table_name=`get_table_name $application entity`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,login_name char (20),city char (20),state_code char (2),zip_code char (10),phone_number char (15),cell_number char (15),email_address char (50)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql
echo "create unique index ${table_name}_additional_unique on $table_name (login_name);" | sql.e '^' mysql
table_name=`get_table_name $application federal_income_tax_withholding`
echo "create table $table_name (federal_marital_status char (25) not null,income_over integer not null,income_not_over integer,tax_fixed_amount double (10,2),tax_percentage_amount double (5,1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (federal_marital_status,income_over);" | sql.e '^' mysql
table_name=`get_table_name $application federal_marital_status`
echo "create table $table_name (federal_marital_status char (25) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (federal_marital_status);" | sql.e '^' mysql
table_name=`get_table_name $application fixed_asset`
echo "create table $table_name (asset_name char (30) not null,account char (60)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (asset_name);" | sql.e '^' mysql
table_name=`get_table_name $application fixed_asset_purchase`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,purchase_date_time datetime not null,asset_name char (30) not null,serial_number char (10) not null,extension double (10,2),estimated_useful_life_years integer,estimated_useful_life_units integer,estimated_residual_value double (10,2),declining_balance_n integer,depreciation_method char (25),accumulated_depreciation double (10,2),disposal_date date) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,asset_name,serial_number);" | sql.e '^' mysql
table_name=`get_table_name $application fixed_service`
echo "create table $table_name (service_name char (30) not null,retail_price double (10,2),service_category char (30),account char (60)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (service_name);" | sql.e '^' mysql
table_name=`get_table_name $application fixed_service_category`
echo "create table $table_name (service_category char (30) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (service_category);" | sql.e '^' mysql
table_name=`get_table_name $application fixed_service_sale`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,retail_price double (10,2),discount_amount double (10,2),work_hours double (10,4),extension double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name);" | sql.e '^' mysql
table_name=`get_table_name $application fixed_service_work`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,begin_work_date_time datetime not null,end_work_date_time datetime,login_name char (20),work_hours double (10,4)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name,begin_work_date_time);" | sql.e '^' mysql
table_name=`get_table_name $application hourly_service`
echo "create table $table_name (service_name char (30) not null,hourly_rate double (7,2),service_category char (30),account char (60)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (service_name);" | sql.e '^' mysql
table_name=`get_table_name $application hourly_service_category`
echo "create table $table_name (service_category char (30) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (service_category);" | sql.e '^' mysql
table_name=`get_table_name $application hourly_service_sale`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,description char (60) not null,estimated_hours double (6,2),hourly_rate double (7,2),work_hours double (10,4),extension double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name,description);" | sql.e '^' mysql
table_name=`get_table_name $application hourly_service_work`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sale_date_time datetime not null,service_name char (30) not null,description char (60) not null,begin_work_date_time datetime not null,end_work_date_time datetime,login_name char (20),work_hours double (10,4)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,service_name,description,begin_work_date_time);" | sql.e '^' mysql
table_name=`get_table_name $application inventory`
echo "create table $table_name (inventory_name char (30) not null,inventory_account char (60),cost_of_goods_sold_account char (60),retail_price double (10,2),inventory_category char (30),reorder_quantity integer,quantity_on_hand integer,average_unit_cost double (12,4),total_cost_balance double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (inventory_name);" | sql.e '^' mysql
table_name=`get_table_name $application inventory_category`
echo "create table $table_name (inventory_category char (30) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (inventory_category);" | sql.e '^' mysql
table_name=`get_table_name $application inventory_cost_method`
echo "create table $table_name (inventory_cost_method char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (inventory_cost_method);" | sql.e '^' mysql
table_name=`get_table_name $application inventory_purchase`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,purchase_date_time datetime not null,inventory_name char (30) not null,ordered_quantity integer,arrived_quantity integer,missing_quantity integer,quantity_on_hand integer,capitalized_unit_cost double (10,4),unit_cost double (10,2),extension double (10,2),average_unit_cost double (12,4)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,inventory_name);" | sql.e '^' mysql
table_name=`get_table_name $application inventory_sale`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,sale_date_time datetime not null,inventory_name char (30) not null,quantity integer,retail_price double (10,2),discount_amount double (10,2),extension double (10,2),cost_of_goods_sold double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,sale_date_time,inventory_name);" | sql.e '^' mysql
table_name=`get_table_name $application journal_ledger`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,transaction_date_time datetime not null,account char (60) not null,transaction_count integer,previous_balance double (10,2),debit_amount double (10,2),credit_amount double (10,2),balance double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,transaction_date_time,account);" | sql.e '^' mysql
table_name=`get_table_name $application liability_account_entity`
echo "create table $table_name (account char (60) not null,full_name char (50),street_address char (40)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (account);" | sql.e '^' mysql
table_name=`get_table_name $application payroll_pay_period`
echo "create table $table_name (payroll_pay_period char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (payroll_pay_period);" | sql.e '^' mysql
table_name=`get_table_name $application payroll_posting`
echo "create table $table_name (payroll_year integer not null,payroll_period_number integer not null,begin_work_date date,end_work_date date,regular_work_hours double (6,2),overtime_work_hours double (6,2),gross_pay double (10,2),net_pay double (10,2),payroll_tax_amount double (10,2),commission_sum_extension double (10,2),federal_tax_withholding_amount double (10,2),state_tax_withholding_amount double (10,2),social_security_employee_tax_amount double (10,2),social_security_employer_tax_amount double (10,2),medicare_employee_tax_amount double (10,2),medicare_employer_tax_amount double (10,2),retirement_contribution_plan_employee_amount integer,retirement_contribution_plan_employer_amount integer,health_insurance_employee_amount integer,health_insurance_employer_amount integer,federal_unemployment_tax_amount double (10,2),state_unemployment_tax_amount double (10,2),union_dues_amount integer) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (payroll_year,payroll_period_number);" | sql.e '^' mysql
table_name=`get_table_name $application prior_fixed_asset`
echo "create table $table_name (asset_name char (30) not null,serial_number char (10) not null,extension double (10,2),recorded_date date,estimated_useful_life_years integer,estimated_useful_life_units integer,estimated_residual_value double (10,2),declining_balance_n integer,depreciation_method char (25),disposal_date date,full_name char (50),street_address char (40),transaction_date_time datetime,accumulated_depreciation double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (asset_name,serial_number);" | sql.e '^' mysql
table_name=`get_table_name $application prior_fixed_asset_depreciation`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,asset_name char (30) not null,serial_number char (10) not null,depreciation_date date not null,depreciation_amount double (10,2),transaction_date_time datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,asset_name,serial_number,depreciation_date);" | sql.e '^' mysql
table_name=`get_table_name $application purchase_order`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,purchase_date_time datetime not null,amount_due double (10,2),sum_extension double (10,2),sales_tax double (7,2),freight_in double (6,2),purchase_amount double (10,2),title_passage_rule char (15),shipped_date date,arrived_date_time datetime,transaction_date_time datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time);" | sql.e '^' mysql
table_name=`get_table_name $application receipt_upload`
echo "create table $table_name (receipt_upload_filename char (80) not null,upload_date date,posted_yn char (1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (receipt_upload_filename);" | sql.e '^' mysql
table_name=`get_table_name $application reoccurring_transaction`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,debit_account char (60),credit_account char (60),transaction_amount double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql
table_name=`get_table_name $application self`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,inventory_cost_method char (15),payroll_pay_period char (15),payroll_beginning_day char (9),social_security_combined_tax_rate double (10,4),social_security_payroll_ceiling integer,medicare_combined_tax_rate double (10,4),medicare_additional_withholding_rate double (5,3),medicare_additional_gross_pay_floor integer,federal_withholding_allowance_period_value double (10,2),federal_nonresident_withholding_income_premium double (7,2),state_withholding_allowance_period_value char (7),state_itemized_allowance_period_value double (10,2),federal_unemployment_wage_base integer,federal_unemployment_tax_standard_rate double (10,4),federal_unemployment_threshold_rate double (10,4),federal_unemployment_tax_minimum_rate double (10,4),state_unemployment_wage_base integer,state_unemployment_tax_rate double (10,4),state_sales_tax_rate double (6,4)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql
table_name=`get_table_name $application service_purchase`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,purchase_date_time datetime not null,account char (60) not null,service_description text,extension double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,account);" | sql.e '^' mysql
table_name=`get_table_name $application state_income_tax_withholding`
echo "create table $table_name (state_marital_status char (40) not null,income_over integer not null,income_not_over integer,tax_fixed_amount double (10,2),tax_percentage_amount double (5,1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (state_marital_status,income_over);" | sql.e '^' mysql
table_name=`get_table_name $application state_marital_status`
echo "create table $table_name (state_marital_status char (40) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (state_marital_status);" | sql.e '^' mysql
table_name=`get_table_name $application state_standard_deduction_table`
echo "create table $table_name (state_marital_status char (40) not null,state_withholding_allowances integer not null,state_standard_deduction_amount double (8,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (state_marital_status,state_withholding_allowances);" | sql.e '^' mysql
table_name=`get_table_name $application subclassification`
echo "create table $table_name (subclassification char (35) not null,element char (20),display_order integer) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (subclassification);" | sql.e '^' mysql
table_name=`get_table_name $application supply`
echo "create table $table_name (supply_name char (30) not null,account char (60)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (supply_name);" | sql.e '^' mysql
table_name=`get_table_name $application supply_purchase`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,purchase_date_time datetime not null,supply_name char (30) not null,quantity integer,unit_cost double (10,2),extension double (10,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,supply_name);" | sql.e '^' mysql
table_name=`get_table_name $application tax_form`
echo "create table $table_name (tax_form char (20) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (tax_form);" | sql.e '^' mysql
table_name=`get_table_name $application tax_form_line`
echo "create table $table_name (tax_form char (20) not null,tax_form_line char (5) not null,tax_form_description char (35),itemize_accounts_yn char (1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (tax_form,tax_form_line);" | sql.e '^' mysql
table_name=`get_table_name $application tax_form_line_account`
echo "create table $table_name (tax_form char (20) not null,tax_form_line char (5) not null,account char (60) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (tax_form,tax_form_line,account);" | sql.e '^' mysql
table_name=`get_table_name $application title_passage_rule`
echo "create table $table_name (title_passage_rule char (15) not null,title_passage_date char (15)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (title_passage_rule);" | sql.e '^' mysql
table_name=`get_table_name $application transaction`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,transaction_date_time datetime not null,memo char (60),transaction_amount double (10,2),check_number integer,lock_transaction_yn char (1)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,transaction_date_time);" | sql.e '^' mysql
echo "create unique index ${table_name}_additional_unique on $table_name (transaction_date_time);" | sql.e '^' mysql
table_name=`get_table_name $application vendor`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql
table_name=`get_table_name $application vendor_payment`
echo "create table $table_name (full_name char (50) not null,street_address char (40) not null,purchase_date_time datetime not null,payment_date_time datetime not null,payment_amount double (10,2),check_number integer,transaction_date_time datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,purchase_date_time,payment_date_time);" | sql.e '^' mysql

(
cat << all_done2
insert into element (element,accumulate_debit_yn) values ('asset','y');
insert into element (element,accumulate_debit_yn) values ('equity','n');
insert into element (element,accumulate_debit_yn) values ('expense','y');
insert into element (element,accumulate_debit_yn) values ('gain','n');
insert into element (element,accumulate_debit_yn) values ('liability','n');
insert into element (element,accumulate_debit_yn) values ('loss','y');
insert into element (element,accumulate_debit_yn) values ('revenue','n');
insert into subclassification (subclassification,element,display_order) values ('cash','asset','1');
insert into subclassification (subclassification,element,display_order) values ('contributed capital','equity','15');
insert into subclassification (subclassification,element,display_order) values ('cost_of_goods_sold','expense','11');
insert into subclassification (subclassification,element,display_order) values ('current_liability','liability','6');
insert into subclassification (subclassification,element,display_order) values ('gain','gain','13');
insert into subclassification (subclassification,element,display_order) values ('inventory','asset','2');
insert into subclassification (subclassification,element,display_order) values ('long-term investment','asset','4');
insert into subclassification (subclassification,element,display_order) values ('long-term liability','liability','7');
insert into subclassification (subclassification,element,display_order) values ('loss','loss','14');
insert into subclassification (subclassification,element,display_order) values ('non-cash current asset','asset','3');
insert into subclassification (subclassification,element,display_order) values ('operating_expense','expense','9');
insert into subclassification (subclassification,element,display_order) values ('property_plant_equipment','asset','5');
insert into subclassification (subclassification,element,display_order) values ('retained earnings','equity','16');
insert into subclassification (subclassification,element,display_order) values ('revenue','revenue','8');
insert into subclassification (subclassification,element,display_order) values ('supply_expense','expense','10');
insert into subclassification (subclassification,element,display_order) values ('tax_expense','expense','12');
insert into account (account,subclassification,hard_coded_account_key) values ('accounting','operating_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('account_payable','current_liability','account_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('account_receivable','non-cash current asset','account_receivable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('accumulated_depreciation','property_plant_equipment','accumulated_depreciation_key');
insert into account (account,subclassification,hard_coded_account_key) values ('advertising','operating_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('checking','cash','cash_key');
insert into account (account,subclassification,hard_coded_account_key) values ('cost_of_goods_sold','cost_of_goods_sold','cost_of_goods_sold_key');
insert into account (account,subclassification,hard_coded_account_key) values ('depreciation_expense','operating_expense','depreciation_expense_key');
insert into account (account,subclassification,hard_coded_account_key) values ('federal_unemployment_tax_payable','current_liability','federal_unemployment_tax_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('federal_withholding_payable','current_liability','federal_withholding_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('fixed_asset','property_plant_equipment',null);
insert into account (account,subclassification,hard_coded_account_key) values ('freight_in_expense','operating_expense','freight_in_key');
insert into account (account,subclassification,hard_coded_account_key) values (' equity','contributed capital',null);
insert into account (account,subclassification,hard_coded_account_key) values ('land','property_plant_equipment','land_key');
insert into account (account,subclassification,hard_coded_account_key) values ('loss','loss','loss_key');
insert into account (account,subclassification,hard_coded_account_key) values ('medicare_payable','current_liability','medicare_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('inventory','inventory',null);
insert into account (account,subclassification,hard_coded_account_key) values ('payroll_payable','current_liability','payroll_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('payroll_tax','tax_expense','payroll_tax_key');
insert into account (account,subclassification,hard_coded_account_key) values ('repairs_maintenance_expense','operating_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('salary_wage_expense','operating_expense','salary_wage_expense_key');
insert into account (account,subclassification,hard_coded_account_key) values ('sales_revenue','revenue','sales_revenue_key');
insert into account (account,subclassification,hard_coded_account_key) values ('sales_tax_expense','tax_expense','sales_tax_key');
insert into account (account,subclassification,hard_coded_account_key) values ('sales_tax_payable','current_liability','sales_tax_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('service_revenue','revenue',null);
insert into account (account,subclassification,hard_coded_account_key) values ('social_security_payable','current_liability','social_security_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('specific_inventory','inventory','specific_inventory_key');
insert into account (account,subclassification,hard_coded_account_key) values ('state_unemployment_tax_payable','current_liability','state_unemployment_tax_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('state_withholding_payable','current_liability','state_withholding_payable_key');
insert into account (account,subclassification,hard_coded_account_key) values ('supply_expense','tax_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('taxes_and_licenses','tax_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('towels','supply_expense',null);
insert into account (account,subclassification,hard_coded_account_key) values ('uncleared_checks','current_liability','uncleared_checks_key');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','001','sales_revenue');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','001','service_revenue');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','004','cost_of_goods_sold');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','008','advertising');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','013','depreciation_expense');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','017','accounting');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','021','repairs_maintenance_expense');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','022','supply_expense');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','022','towels');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','023','payroll_tax');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','023','sales_tax_expense');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','023','taxes_and_licenses');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','026','salary_wage_expense');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','041','inventory');
insert into tax_form_line_account (tax_form,tax_form_line,account) values ('Schedule C','041','specific_inventory');
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','001','Gross receipts or sales',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','002','Returns and allowances',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','004','Cost of goods sold',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','008','Advertising',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','009','Car and truck expenses',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','013','Depreciation',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','015','Insurance',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','016a','Mortgage interest',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','016b','Other interest',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','017','Legal and professional services',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','018','Office expense',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','020a','Vehicle, machinery, equipment rent',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','020b','Property rent',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','021','Repairs and maintenance',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','022','Supplies',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','023','Taxes and licenses',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','024a','Travel expenses',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','024b','Meals and entertainment',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','025','Utilities',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','026','Wages',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','027','Other expenses',null);
insert into tax_form_line (tax_form,tax_form_line,tax_form_description,itemize_accounts_yn) values ('Schedule C','041','Ending inventory',null);
insert into tax_form (tax_form) values ('Schedule C');
insert into day (day) values ('friday');
insert into day (day) values ('monday');
insert into day (day) values ('saturday');
insert into day (day) values ('sunday');
insert into day (day) values ('thursday');
insert into day (day) values ('tuesday');
insert into day (day) values ('wednesday');
insert into payroll_pay_period (payroll_pay_period) values ('biweekly');
insert into payroll_pay_period (payroll_pay_period) values ('monthly');
insert into payroll_pay_period (payroll_pay_period) values ('semimonthly');
insert into payroll_pay_period (payroll_pay_period) values ('weekly');
insert into inventory_cost_method (inventory_cost_method) values ('average');
insert into inventory_cost_method (inventory_cost_method) values ('FIFO');
insert into inventory_cost_method (inventory_cost_method) values ('LIFO');
insert into federal_marital_status (federal_marital_status) values ('married');
insert into federal_marital_status (federal_marital_status) values ('married_but_single_rate');
insert into federal_marital_status (federal_marital_status) values ('single');
insert into state_marital_status (state_marital_status) values ('married_one_income');
insert into state_marital_status (state_marital_status) values ('single_or_married_with_multiple_incomes');
insert into state_marital_status (state_marital_status) values ('unmarried_head_of_household');
insert into depreciation_method (depreciation_method) values ('double_declining_balance');
insert into depreciation_method (depreciation_method) values ('n_declining_balance');
insert into depreciation_method (depreciation_method) values ('straight_line');
insert into depreciation_method (depreciation_method) values ('sum_of_years_digits');
insert into depreciation_method (depreciation_method) values ('units_of_production');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married','166','525','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married','525','1626','35.90','15.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married','1626','3111','201.05','25.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married','3111','4654','572.30','28.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married','4654','8180','1004.34','33.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married','8180','9218','2167.92','35.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married','9218','0','2531.22','39.6');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_but_single_rate','44','224','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_but_single_rate','224','774','18.00','15.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_but_single_rate','774','1812','100.50','25.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_but_single_rate','1812','3730','360.00','28.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_but_single_rate','3730','8058','897.04','33.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_but_single_rate','8058','8090','2325.28','35.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_but_single_rate','8090','0','2336.48','39.6');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single','44','224','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single','224','774','18.00','15.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single','774','1812','100.50','25.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single','1812','3730','360.00','28.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single','3730','8058','897.04','33.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single','8058','8090','2325.28','35.0');
insert into federal_income_tax_withholding (federal_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single','8090','0','2336.48','39.6');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','0','308','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','308','730','3.39','2.2');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','730','1154','12.67','4.4');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','1154','1602','31.33','6.6');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','1602','2024','60.90','8.8');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','2024','10336','98.04','10.2');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','10336','12404','948.69','11.3');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','12404','19231','1182.66','12.4');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','19231','20673','2031.26','13.5');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('married_one_income','20673','0','2226.36','14.6');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','0','154','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','154','365','1.69','2.0');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','354','577','6.33','4.4');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','577','801','15.66','6.6');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','801','1012','30.44','8.8');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','1012','5168','49.01','10.2');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','5169','6202','474.17','11.3');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','6202','10337','591.32','12.4');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','10337','19231','1105.30','13.5');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('single_or_married_with_multiple_incomes','19231',null,'2308.66','14.6');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','0','308','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','308','731','3.39','2.2');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','731','942','12.70','4.4');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','942','1166','21.98','6.6');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','1166','1377','36.76','8.8');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','1377','7029','55.33','10.2');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','7029','8435','633.53','11.3');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','8435','14058','792.83','12.4');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','14058','19231','1491.77','13.5');
insert into state_income_tax_withholding (state_marital_status,
		 income_over,
		 income_not_over,
		 tax_fixed_amount,
		 tax_percentage_amount) values ('unmarried_head_of_household','19231','0','2191.68','14.6');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','0','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','1','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','2','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','3','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','4','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','5','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','6','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','7','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','8','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','9','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('married_one_income','10','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','0','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','1','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','2','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','3','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','4','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','5','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','6','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','7','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','8','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','9','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','10','79.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','0','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','1','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','2','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','3','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','4','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','5','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','6','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','7','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','8','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','9','159.00');
insert into state_standard_deduction_table (state_marital_status,
		 state_withholding_allowances,
		 state_standard_deduction_amount) values ('unmarried_head_of_household','10','159.00');
insert into fixed_service_category (service_category) values ('standard');
insert into hourly_service_category (service_category) values ('standard');
all_done2
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done3
insert into role_operation (folder,role,operation) values ('account','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('account','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('account','bookkeeper','ledger_propagate');
insert into role_operation (folder,role,operation) values ('account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('account','supervisor','ledger_propagate');
insert into role_operation (folder,role,operation) values ('bank_upload','supervisor','delete');
insert into role_operation (folder,role,operation) values ('contra_account','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('contra_account','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('contra_account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('contra_account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('customer','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('customer','bookkeeper','delete_isa_only');
insert into role_operation (folder,role,operation) values ('customer','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('customer','supervisor','delete');
insert into role_operation (folder,role,operation) values ('customer','supervisor','delete_isa_only');
insert into role_operation (folder,role,operation) values ('customer','supervisor','detail');
insert into role_operation (folder,role,operation) values ('customer_payment','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('customer_payment','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('customer_payment','supervisor','delete');
insert into role_operation (folder,role,operation) values ('customer_payment','supervisor','detail');
insert into role_operation (folder,role,operation) values ('customer_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('customer_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('customer_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('customer_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('day','supervisor','delete');
insert into role_operation (folder,role,operation) values ('depreciation','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('depreciation','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('depreciation','supervisor','detail');
insert into role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('depreciation_method','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('element','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('element','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('element','supervisor','delete');
insert into role_operation (folder,role,operation) values ('element','supervisor','detail');
insert into role_operation (folder,role,operation) values ('employee','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('employee','bookkeeper','delete_isa_only');
insert into role_operation (folder,role,operation) values ('employee','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('employee','supervisor','delete');
insert into role_operation (folder,role,operation) values ('employee','supervisor','delete_isa_only');
insert into role_operation (folder,role,operation) values ('employee','supervisor','detail');
insert into role_operation (folder,role,operation) values ('employee_work_day','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('employee_work_day','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('employee_work_day','supervisor','delete');
insert into role_operation (folder,role,operation) values ('employee_work_day','supervisor','detail');
insert into role_operation (folder,role,operation) values ('employee_work_period','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('employee_work_period','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('employee_work_period','supervisor','delete');
insert into role_operation (folder,role,operation) values ('employee_work_period','supervisor','detail');
insert into role_operation (folder,role,operation) values ('entity','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('entity','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('entity','supervisor','delete');
insert into role_operation (folder,role,operation) values ('entity','supervisor','detail');
insert into role_operation (folder,role,operation) values ('federal_income_tax_withholding','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('federal_income_tax_withholding','supervisor','delete');
insert into role_operation (folder,role,operation) values ('federal_marital_status','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('federal_marital_status','supervisor','delete');
insert into role_operation (folder,role,operation) values ('federal_marital_status','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_asset_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_service','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_category','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_category','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_category','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_work','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_work','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('fixed_service_work','supervisor','delete');
insert into role_operation (folder,role,operation) values ('fixed_service_work','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('hourly_service','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_category','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_category','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_category','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_work','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_work','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('hourly_service_work','supervisor','delete');
insert into role_operation (folder,role,operation) values ('hourly_service_work','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory_category','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_category','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory_category','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory_cost_method','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory_cost_method','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('inventory_sale','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('inventory_sale','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('inventory_sale','supervisor','delete');
insert into role_operation (folder,role,operation) values ('inventory_sale','supervisor','detail');
insert into role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('journal_ledger','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('journal_ledger','supervisor','delete');
insert into role_operation (folder,role,operation) values ('journal_ledger','supervisor','detail');
insert into role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('liability_account_entity','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('liability_account_entity','supervisor','delete');
insert into role_operation (folder,role,operation) values ('liability_account_entity','supervisor','detail');
insert into role_operation (folder,role,operation) values ('payroll_pay_period','supervisor','delete');
insert into role_operation (folder,role,operation) values ('payroll_posting','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('payroll_posting','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','supervisor','delete');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset','supervisor','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset_depreciation','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('prior_fixed_asset_depreciation','supervisor','detail');
insert into role_operation (folder,role,operation) values ('purchase_order','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('purchase_order','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('purchase_order','supervisor','delete');
insert into role_operation (folder,role,operation) values ('purchase_order','supervisor','detail');
insert into role_operation (folder,role,operation) values ('receipt_upload','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('receipt_upload','supervisor','delete');
insert into role_operation (folder,role,operation) values ('reoccurring_transaction','supervisor','delete');
insert into role_operation (folder,role,operation) values ('reoccurring_transaction','supervisor','detail');
insert into role_operation (folder,role,operation) values ('self','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('self','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('service_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('service_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('service_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('service_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','supervisor','delete');
insert into role_operation (folder,role,operation) values ('state_income_tax_withholding','supervisor','detail');
insert into role_operation (folder,role,operation) values ('state_marital_status','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('state_marital_status','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('state_marital_status','supervisor','delete');
insert into role_operation (folder,role,operation) values ('state_marital_status','supervisor','detail');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','supervisor','delete');
insert into role_operation (folder,role,operation) values ('state_standard_deduction_table','supervisor','detail');
insert into role_operation (folder,role,operation) values ('subclassification','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('subclassification','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('subclassification','supervisor','delete');
insert into role_operation (folder,role,operation) values ('subclassification','supervisor','detail');
insert into role_operation (folder,role,operation) values ('supply','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('supply','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('supply','supervisor','delete');
insert into role_operation (folder,role,operation) values ('supply','supervisor','detail');
insert into role_operation (folder,role,operation) values ('supply_purchase','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('supply_purchase','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('supply_purchase','supervisor','delete');
insert into role_operation (folder,role,operation) values ('supply_purchase','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_form','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('tax_form','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_form','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_form','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line','supervisor','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','supervisor','delete');
insert into role_operation (folder,role,operation) values ('tax_form_line_account','supervisor','detail');
insert into role_operation (folder,role,operation) values ('title_passage_rule','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('transaction','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('transaction','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('transaction','supervisor','delete');
insert into role_operation (folder,role,operation) values ('transaction','supervisor','detail');
insert into role_operation (folder,role,operation) values ('vendor','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('vendor','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('vendor','supervisor','delete');
insert into role_operation (folder,role,operation) values ('vendor','supervisor','detail');
insert into role_operation (folder,role,operation) values ('vendor_payment','bookkeeper','delete');
insert into role_operation (folder,role,operation) values ('vendor_payment','bookkeeper','detail');
insert into role_operation (folder,role,operation) values ('vendor_payment','supervisor','delete');
insert into role_operation (folder,role,operation) values ('vendor_payment','supervisor','detail');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('add_column','add_column \$application \$session \$login_name \$role folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('alter_column_datatype','alter_column_datatype \$application \$session \$login_name \$role folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('appaserver_info','appaserver_info.sh \$application \$login_name',null,null,null,null,'documentation',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('attribute_list','attribute_list \$application \$dictionary',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('balance_sheet','balance_sheet \$application \$process fund as_of_date aggregate_subclassification_yn output_medium',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('change_password','maintain_user_account \$application \$session \$person \$role',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('clone_application','clone_application \$application \$session \$login_name \$role destination_application delete_yn system_folders_yn really_yn output2file_yn database_management_system export_output',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('clone_folder','clone_folder \$application n \$session \$login_name \$role destination_application folder attribute old_data new_data html delete_yn execute_yn database_management_system output2file_yn',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('close_fixed_service_work','shortcut_service_work \$application \$process \$login_name full_name street_address sale_date_time close fixed service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('close_hourly_service_work','shortcut_service_work \$application \$process \$login_name full_name street_address sale_date_time close hourly service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('close_nominal_accounts','close_nominal_accounts \$application  \$process as_of_date execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('collect_customer_payment','collect_customer_payment \$application \$process full_name street_address sale_date_time payment_amount check_number paid_amount_due_yn',null,null,null,null,'shortcut',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('create_application','create_application \$application build_shell_script_yn \$login_name \$role destination_application system_folders_yn output2file_yn really_yn database_management_system',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('create_empty_application','create_empty_application \$application \$session \$login_name \$role \$process destination_application new_application_title y delete_application_yn execute_yn','This process creates an empty application. It creates a new database, the Appaserver tables, the data directories, among other application objects. Following this process, you can begin inserting the folder rows.',null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('create_table','create_table \$application n \$session \$login_name \$role destination_application folder really_yn database_management_system',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('delete','delete_folder_row \$application \$session \$person \$folder \$role \$primary_data_list sql.e n',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('delete_isa_only','delete_folder_row \$application \$session \$person \$folder \$role \$primary_data_list sql.e y',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('depreciate_fixed_assets','depreciate_fixed_assets \$application \$process fund undo_yn execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('depreciate_prior_fixed_assets','depreciate_prior_fixed_assets \$application \$process fund undo_yn execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('detail','detail \$application \$session \$login_name \$folder \$role \$target_frame \$primary_data_list \$dictionary',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('drop_column','drop_column \$application \$session \$login_name \$role folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('execute_select_statement','execute_select_statement \$application \$process \$login_name \$session \$role output_medium filename select_statement_title login_name',null,null,null,null,'output',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('export_application','clone_application \$application \$session \$login_name \$role destination_application delete_yn system_folders_yn really_yn output2file_yn database_management_system export_output',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('export_folder','clone_folder \$application \$session \$login_name \$role destination_application folder attribute old_data new_data html n really_yn database_management_system y export_output',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('export_process','export_process \$application \$session \$login_name \$role \$process \$dictionary','For a list of processes, this process exports from the following: process, role_process, process_parameter, javascript_libraries, prompt, drop_down_prompt, and drop_down_prompt_data.',null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('export_process_set','export_process_set \$application \$session \$login_name \$role \$process \$dictionary','For a list of process sets, this process exports from the following: process_set, process_set_parameter, javascript_libraries, role_process_set, prompt, drop_down_prompt, and drop_down_prompt_data.',null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('export_subschema','export_subschema \$application \$session \$login_name \$role destination_application \$dictionary','This process exports for a list of folders from the following: folder, relation, folder_attribute, attribute, folder_operation, role_operation, and role_folder.',null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('fix_orphans','fix_orphans \$application \$process orphans_folder delete_yn really_yn','This process traverses the many-to-one relationships for a folder. It inserts the missing primary keys that contain foreign keys in the selected folder. Warning: this process could take a long time to run.',null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('generate_invoice','generate_invoice \$application \$process full_name street_address sale_date_time',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('generate_workorder','generate_workorder \$application \$process full_name street_address sale_date_time',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('generic_load','generic_load \$application \$session \$role \$process',null,null,null,null,'load',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('google_map','google_map_operation \$application \$process \$login_name \$role \$folder latitude longitude utm_easting utm_northing \$process_id \$process_row_count \$session \$dictionary',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('grant_select_to_user','grant_select_to_user \$application \$process login_name connect_from_host revoke_only_yn really_yn',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('income_statement','income_statement \$application \$process fund as_of_date aggregate_subclassification_yn output_medium',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('inventory_balance_detail','inventory_balance_detail \$application \$process inventory_name output_medium',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('inventory_propagate','post_change_inventory_purchase \$application full_name street_address purchase_date_time inventory_name preupdate_purchase_date_time preupdate_inventory_name preupdate_arrived_quantity \$state',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('ledger_propagate','ledger_propagate \$application transaction_date_time preupdate_transaction_date_time account preupdate_account',null,null,null,null,'manipulate',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('load_bank_spreadsheet','load_bank_spreadsheet \$application \$process fund filename execute_yn',null,null,null,null,'load',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('load_receipt_scan','load_receipt_scan \$application \$process fund filename',null,null,null,null,'load',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('merge_purge','merge_purge \$application \$session \$role \$process',null,null,null,null,'manipulate',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('null',null,null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('open_fixed_service_work','shortcut_service_work \$application \$process \$login_name full_name street_address sale_date_time open fixed service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('open_hourly_service_work','shortcut_service_work \$application \$process \$login_name full_name street_address sale_date_time open hourly service_name description begin_work_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('orphans_process_folder_list','orphans_process_folder_list.sh \$application',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('pay_liabilities','pay_liabilities_process \$application \$process \$session fund full_name street_address starting_check_number memo payment_amount with_stub_yn execute_yn',null,null,'post_change_pay_liabilities( this )',null,'manipulate',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('populate_account','populate_account.sh \$application \$one2m_folder subclassification',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('populate_entity_reoccurring_transaction','populate_entity_reoccurring_transaction.sh \$application',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('populate_inventory','populate_inventory.sh \$application inventory_category',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('populate_print_checks_entity','populate_print_checks_entity \$application fund',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('populate_tax_form_line','populate_tax_form_line.sh \$application \$where',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_bank_upload','post_change_bank_upload \$application \$dictionary',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_customer_payment','post_change_customer_payment \$application full_name street_address sale_date_time payment_date_time \$state preupdate_payment_date_time preupdate_payment_amount',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_customer_sale','post_change_customer_sale \$application full_name street_address sale_date_time \$state preupdate_full_name preupdate_street_address preupdate_title_passage_rule preupdate_completed_date_time preupdate_shipped_date_time preupdate_arrived_date preupdate_shipping_revenue',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_employee_work_day','post_change_employee_work_day \$application full_name street_address begin_work_date_time \$state preupdate_begin_work_date_time preupdate_end_work_date_time',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_fixed_asset_purchase','post_change_fixed_asset_purchase \$application full_name street_address purchase_date_time asset_name serial_number \$state preupdate_asset_name preupdate_serial_number preupdate_extension preupdate_declining_balance_n preupdate_depreciation_method',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_fixed_service_sale','post_change_fixed_service_sale \$application full_name street_address sale_date_time service_name \$state',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_fixed_service_work','post_change_fixed_service_work \$application full_name street_address sale_date_time service_name begin_work_date_time \$state preupdate_end_work_date_time',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_hourly_service_sale','post_change_hourly_service_sale \$application full_name street_address sale_date_time service_name description \$state',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_hourly_service_work','post_change_hourly_service_work \$application full_name street_address sale_date_time service_name description begin_work_date_time \$state preupdate_end_work_date_time',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_inventory_purchase','post_change_inventory_purchase \$application full_name street_address purchase_date_time inventory_name \$state preupdate_ordered_quantity preupdate_arrived_quantity preupdate_missing_quantity preupdate_unit_cost preupdate_inventory_name',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_inventory_sale','post_change_inventory_sale \$application full_name street_address sale_date_time inventory_name \$state preupdate_quantity preupdate_retail_price preupdate_discount_amount preupdate_inventory_name',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_journal_ledger','post_change_journal_ledger.sh \$application full_name street_address transaction_date_time account preupdate_transaction_date_time preupdate_account',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_purchase_order','post_change_purchase_order \$application full_name street_address purchase_date_time \$state preupdate_full_name preupdate_street_address preupdate_title_passage_rule preupdate_shipped_date preupdate_arrived_date_time preupdate_sales_tax preupdate_freight_in',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_self','post_change_self \$application preupdate_inventory_cost_method',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_supply_purchase','post_change_supply_purchase \$application full_name street_address purchase_date_time supply_name \$state preupdate_supply_name',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_change_vendor_payment','post_change_vendor_payment \$application full_name street_address purchase_date_time payment_date_time \$state preupdate_full_name preupdate_street_address preupdate_payment_date_time preupdate_payment_amount',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_payroll','post_employee_work_period \$application \$process payroll_year period_number delete_yn y execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('post_reoccurring_transaction','post_reoccurring_transaction \$application \$process full_name street_address transaction_date transaction_amount memo execute_yn',null,null,null,null,'manipulate',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('recalculate_cost','post_change_inventory_sale \$application full_name street_address sale_date_time inventory_name quantity retail_price discount_amount \$state',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('rename_column','rename_column \$application \$process old_attribute folder attribute really_yn',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('rename_table','rename_table \$application \$session \$login_name \$role old_folder new_folder really_yn',null,null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('report_writer','post_report_writer \$application \$session \$login_name \$role \$process folder one',null,null,null,null,'output',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('select_closed_customer_sale','select_customer_sale.sh \$application \$state closed',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('select_customer_sale_collect_payment','select_customer_sale_collect_customer_payment.sh \$application',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('select_open_customer_sale','select_customer_sale.sh \$application \$state open',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('select_open_fixed_service_sale','select_open_fixed_service_sale.sh \$application',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('select_open_fixed_service_work','select_open_fixed_service_work.sh \$application \$login_name',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('select_open_hourly_service_sale','select_open_hourly_service_sale.sh \$application',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('select_open_hourly_service_work','select_open_hourly_service_work.sh \$application \$login_name',null,null,null,null,null,null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('table_rectification','table_rectification \$application \$session \$login_name \$role','This process compares the Appaserver attributes with the Mysql table columns. It then gives you the opportunity to drop the residual columns.',null,null,null,null,null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('tax_form_report','tax_form_report \$application \$process tax_form as_of_date output_medium',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('trial_balance','trial_balance \$application \$session \$login_name \$role \$process fund as_of_date aggregation output_medium',null,null,null,null,'output',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('update_sale_completed','update_sale_completed \$application \$process full_name street_address sale_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('update_sale_not_completed','update_sale_not_completed \$application \$process full_name street_address sale_date_time',null,null,null,null,'shortcut',null,null);
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('upload_source_file','upload_source_file \$application \$process filename','This process allows you to upload non-executable source files, like javascript. The destination directory is likely \$APPASERVER_HOME/src_\$application.',null,null,null,'load',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('view_appaserver_log_file','view_appaserver_error_file.sh \$application /var/log/appaserver line_count','Most error messages are logged to this file. If you get the Server Error screen, then this file is the first place to find a clue. <big> <bold>Warning:</bold></big> this process exposes the session number assigned to each user''s login.',null,null,null,'output',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('view_documentation','view_diagrams \$application',null,null,null,null,'documentation',null,'y');
insert into process (process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn) values ('view_source','view_source \$application \$session \$login_name \$role',null,null,null,null,'documentation',null,'y');
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('add_column','attribute','null','null','null','1',null,'y',null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('add_column','folder_attribute','null','null','null','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('add_column','null','null','null','execute_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('alter_column_datatype','attribute','null','null','null','1',null,'y',null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('alter_column_datatype','folder_attribute','null','null','null','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('alter_column_datatype','null','null','null','execute_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('balance_sheet','null','null','null','aggregate_subclassification_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('balance_sheet','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('balance_sheet','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_application','application','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_application','null','null','null','delete_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_application','null','null','null','execute_yn','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_application','null','null','null','system_folders_yn','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','application','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','attribute','null','null','null','1',null,'y',null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','folder_attribute','null','null','null','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','null','null','null','delete_yn','7',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','null','null','null','new_data','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','null','null','database_management_system','null','8',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','null','null','null','old_data','4',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('clone_folder','null','null','null','output2file_yn','6',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('close_fixed_service_work','fixed_service_work','null','null','null','1',null,null,'select_open_fixed_service_work',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('close_hourly_service_work','hourly_service_work','null','null','null','1',null,null,'select_open_hourly_service_work',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('close_nominal_accounts','null','null','null','execute_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('collect_customer_payment','customer_sale','null','null','null','10',null,null,'select_customer_sale_collect_payment',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('collect_customer_payment','null','null','null','check_number','40',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('collect_customer_payment','null','null','null','paid_amount_due_yn','20',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('collect_customer_payment','null','null','null','payment_amount','30',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_application','application','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_application','null','null','null','build_shell_script_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_application','null','null','null','execute_yn','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_application','null','null','null','system_folders_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_empty_application','null','null','null','delete_application_yn','7',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_empty_application','null','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_empty_application','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_empty_application','null','null','null','new_application_title','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_table','folder','null','null','null','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('create_table','null','null','null','execute_yn','4',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('depreciate_fixed_assets','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('depreciate_fixed_assets','null','null','null','undo_yn','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('depreciate_prior_fixed_assets','null','null','null','execute_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('depreciate_prior_fixed_assets','null','null','null','undo_yn','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('drop_column','folder','null','null','null','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('drop_column','null','null','null','attribute','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('drop_column','null','null','null','execute_yn','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('execute_select_statement','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('execute_select_statement','null','null','execute_select_statement_output_medium','null','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('execute_select_statement','select_statement','null','null','null','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_application','application','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_application','null','null','export_output','null','4',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_application','null','null','null','system_folders_yn','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_folder','application','null','null','destination_application','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_folder','folder','null','null','null','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_folder','null','null','export_output','null','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_process','null','null','null','exclude_roles_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_process','process','null','null','null','1','y',null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_process_set','null','null','null','exclude_roles_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_process_set','process_set','null','null','null','1','y',null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('export_subschema','folder','null','null','null','2','y',null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('fix_orphans','null','null','null','delete_yn','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('fix_orphans','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('fix_orphans','null','null','null','orphans_folder','1',null,null,'orphans_process_folder_list',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('generate_invoice','customer_sale','null','null','null','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('generate_workorder','customer_sale','null','null','null',null,null,null,'select_open_customer_sale',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('grant_select_to_user','appaserver_user','null','null','null','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('grant_select_to_user','null','null','null','connect_from_host','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('grant_select_to_user','null','null','null','execute_yn','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('grant_select_to_user','null','null','null','revoke_only_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('income_statement','null','null','null','aggregate_subclassification_yn','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('income_statement','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('income_statement','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('inventory_balance_detail','inventory','null','null','null','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('inventory_balance_detail','null','null','inventory_balance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('load_bank_spreadsheet','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('load_bank_spreadsheet','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('load_receipt_scan','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('open_fixed_service_work','fixed_service_sale','null','null','null','1',null,null,'select_open_fixed_service_sale',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('open_hourly_service_work','hourly_service_sale','null','null','null','1',null,null,'select_open_hourly_service_sale',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('pay_liabilities','entity','null','null','null','1','y',null,'populate_print_checks_entity',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('pay_liabilities','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('pay_liabilities','null','null','null','memo','4',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('pay_liabilities','null','null','null','payment_amount','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('pay_liabilities','null','null','null','starting_check_number','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('pay_liabilities','null','null','null','with_stub_yn','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_payroll','null','null','null','delete_yn','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_payroll','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_payroll','null','null','null','payroll_year','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_payroll','null','null','null','period_number','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_reoccurring_transaction','entity','null','null','null','1',null,null,'populate_entity_reoccurring_transaction',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_reoccurring_transaction','null','null','null','execute_yn','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_reoccurring_transaction','null','null','null','memo','5',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_reoccurring_transaction','null','null','null','transaction_amount','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('post_reoccurring_transaction','null','null','null','transaction_date','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('rename_column','attribute','null','null','null',null,null,'y',null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('rename_column','folder_attribute','null','null','null','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('rename_column','null','null','null','execute_yn','4',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('rename_column','null','null','null','old_attribute','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('rename_table','folder','null','null','new_folder','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('rename_table','null','null','null','execute_yn','3',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('rename_table','null','null','null','old_folder','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('tax_form_report','null','null','null','as_of_date','2',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('tax_form_report','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('tax_form_report','tax_form','null','null','null','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('trial_balance','null','null','null','as_of_date','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('trial_balance','null','null','finance_output_medium','null','9',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('update_sale_completed','customer_sale','null','null','null','1',null,null,'select_open_customer_sale',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('update_sale_not_completed','customer_sale','null','null','null','1',null,null,'select_closed_customer_sale',null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('upload_source_file','null','null','null','filename','1',null,null,null,null);
insert into process_parameter (process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process) values ('view_appaserver_log_file','null','null','null','line_count',null,null,null,null,null);
insert into role_process (process,role) values ('appaserver_info','bookkeeper');
insert into role_process (process,role) values ('balance_sheet','bookkeeper');
insert into role_process (process,role) values ('change_password','bookkeeper');
insert into role_process (process,role) values ('close_fixed_service_work','bookkeeper');
insert into role_process (process,role) values ('close_hourly_service_work','bookkeeper');
insert into role_process (process,role) values ('close_nominal_accounts','bookkeeper');
insert into role_process (process,role) values ('collect_customer_payment','bookkeeper');
insert into role_process (process,role) values ('depreciate_fixed_assets','bookkeeper');
insert into role_process (process,role) values ('depreciate_prior_fixed_assets','bookkeeper');
insert into role_process (process,role) values ('execute_select_statement','bookkeeper');
insert into role_process (process,role) values ('generate_invoice','bookkeeper');
insert into role_process (process,role) values ('generate_workorder','bookkeeper');
insert into role_process (process,role) values ('generic_load','bookkeeper');
insert into role_process (process,role) values ('income_statement','bookkeeper');
insert into role_process (process,role) values ('inventory_balance_detail','bookkeeper');
insert into role_process (process,role) values ('ledger_propagate','bookkeeper');
insert into role_process (process,role) values ('load_bank_spreadsheet','bookkeeper');
insert into role_process (process,role) values ('load_receipt_scan','bookkeeper');
insert into role_process (process,role) values ('merge_purge','bookkeeper');
insert into role_process (process,role) values ('open_fixed_service_work','bookkeeper');
insert into role_process (process,role) values ('open_hourly_service_work','bookkeeper');
insert into role_process (process,role) values ('pay_liabilities','bookkeeper');
insert into role_process (process,role) values ('post_payroll','bookkeeper');
insert into role_process (process,role) values ('post_reoccurring_transaction','bookkeeper');
insert into role_process (process,role) values ('tax_form_report','bookkeeper');
insert into role_process (process,role) values ('trial_balance','bookkeeper');
insert into role_process (process,role) values ('update_sale_completed','bookkeeper');
insert into role_process (process,role) values ('update_sale_not_completed','bookkeeper');
insert into role_process (process,role) values ('view_documentation','bookkeeper');
insert into role_process (process,role) values ('view_source','bookkeeper');
insert into role_process (process,role) values ('appaserver_info','supervisor');
insert into role_process (process,role) values ('balance_sheet','supervisor');
insert into role_process (process,role) values ('change_password','supervisor');
insert into role_process (process,role) values ('close_fixed_service_work','supervisor');
insert into role_process (process,role) values ('close_hourly_service_work','supervisor');
insert into role_process (process,role) values ('close_nominal_accounts','supervisor');
insert into role_process (process,role) values ('collect_customer_payment','supervisor');
insert into role_process (process,role) values ('depreciate_fixed_assets','supervisor');
insert into role_process (process,role) values ('depreciate_prior_fixed_assets','supervisor');
insert into role_process (process,role) values ('execute_select_statement','supervisor');
insert into role_process (process,role) values ('generate_invoice','supervisor');
insert into role_process (process,role) values ('generate_workorder','supervisor');
insert into role_process (process,role) values ('generic_load','supervisor');
insert into role_process (process,role) values ('income_statement','supervisor');
insert into role_process (process,role) values ('inventory_balance_detail','supervisor');
insert into role_process (process,role) values ('ledger_propagate','supervisor');
insert into role_process (process,role) values ('load_bank_spreadsheet','supervisor');
insert into role_process (process,role) values ('load_receipt_scan','supervisor');
insert into role_process (process,role) values ('merge_purge','supervisor');
insert into role_process (process,role) values ('open_fixed_service_work','supervisor');
insert into role_process (process,role) values ('open_hourly_service_work','supervisor');
insert into role_process (process,role) values ('pay_liabilities','supervisor');
insert into role_process (process,role) values ('post_payroll','supervisor');
insert into role_process (process,role) values ('post_reoccurring_transaction','supervisor');
insert into role_process (process,role) values ('report_writer','supervisor');
insert into role_process (process,role) values ('tax_form_report','supervisor');
insert into role_process (process,role) values ('trial_balance','supervisor');
insert into role_process (process,role) values ('update_sale_completed','supervisor');
insert into role_process (process,role) values ('update_sale_not_completed','supervisor');
insert into role_process (process,role) values ('view_documentation','supervisor');
insert into role_process (process,role) values ('view_source','supervisor');
insert into role_process (process,role) values ('add_column','system');
insert into role_process (process,role) values ('alter_column_datatype','system');
insert into role_process (process,role) values ('appaserver_info','system');
insert into role_process (process,role) values ('change_password','system');
insert into role_process (process,role) values ('clone_application','system');
insert into role_process (process,role) values ('clone_folder','system');
insert into role_process (process,role) values ('create_application','system');
insert into role_process (process,role) values ('create_empty_application','system');
insert into role_process (process,role) values ('create_table','system');
insert into role_process (process,role) values ('drop_column','system');
insert into role_process (process,role) values ('export_application','system');
insert into role_process (process,role) values ('export_folder','system');
insert into role_process (process,role) values ('export_process','system');
insert into role_process (process,role) values ('export_process_set','system');
insert into role_process (process,role) values ('export_subschema','system');
insert into role_process (process,role) values ('fix_orphans','system');
insert into role_process (process,role) values ('generic_load','system');
insert into role_process (process,role) values ('grant_select_to_user','system');
insert into role_process (process,role) values ('merge_purge','system');
insert into role_process (process,role) values ('rename_column','system');
insert into role_process (process,role) values ('rename_table','system');
insert into role_process (process,role) values ('report_writer','system');
insert into role_process (process,role) values ('table_rectification','system');
insert into role_process (process,role) values ('upload_source_file','system');
insert into role_process (process,role) values ('view_appaserver_log_file','system');
insert into role_process (process,role) values ('view_documentation','system');
insert into role_process (process,role) values ('view_source','system');
insert into javascript_processes (process,javascript_filename) values ('clone_folder','clone_folder.js');
insert into javascript_processes (process,javascript_filename) values ('pay_liabilities','post_change_pay_liabilities.js');
insert into javascript_files (javascript_filename) values ('clone_folder.js');
insert into javascript_files (javascript_filename) values ('post_change_bank_upload.js');
insert into javascript_files (javascript_filename) values ('post_change_journal_ledger.js');
insert into javascript_files (javascript_filename) values ('post_change_pay_liabilities.js');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('accumulate_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('aggregate_subclassification_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('as_of_date','10',null,null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('attribute','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('begin_date','10',null,null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('begin_discovery_date','10',null,null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('begin_time','4','Format HHMM',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('begin_work_date','10',null,null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('begin_year','4',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('build_shell_script_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('chart_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('check_number','10',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('compare_station',null,'Used to compare all of the datatypes for a station',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('compare_station|compare_datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('connect_from_host','60',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('containing_date','10','Aggregate the day or week containing this date.',null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('create_database_yn','1','Should the application be stored in a new database or the current database?',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('daily_values_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('date_offset_comma_list','100','Dates to load: 1=Jan 1, 365=Dec 31 or',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('date_or_yesterday','10',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('days_to_average','3','Defaults to 30',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('delete_application_yn','1','Delete the existing application?',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('delete_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('destination_application','30',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('destination_datatype','0',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('destination_directory','100',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('destination_station','0',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('destination_station|destination_datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('detail_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('discovery_date|primary_researcher',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('display_count_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('dynamic_ending_date','10',null,null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('elevation_offset','5','How many feet above or below the station''s ground surface elevation?',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('email_address','50','Optionally email the results',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('email_address_required','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('end_date','10',null,null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('end_time','4','Format HHMM',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('end_year','4',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('exceedance_format_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('exclude_folder_comma_list','255',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('exclude_roles_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('execute_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('filename','100',null,'y',null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('first_value','5',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('flow_units_converted',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('gridlines_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('hours_to_bump','5',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('ignore_closing_entry_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('inflow_station|inflow_datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('last_value','5',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('line_count','4','How many lines from the end to include? Defaults to 50.',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('measurements_per_day','3',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('memo','60',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('minimum_spike','10',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('minutes_offset','2','minutes from midnight to start (0-59)',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('multiplier','5',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('multiply_first_then_add_yn','1','Default is to add first, then multiply.',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('negative_station|negative_datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('nest_number',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('new_application_title','30',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('new_attribute','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('new_data','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('new_folder','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('new_station_datatype_group','30','Add these datatypes to a group with this name',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('notes','30',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('null','0',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('offset','5',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('old_attribute','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('old_data','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('old_folder','50',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('omit_output_if_any_missing_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('optional_folder',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('orphans_folder',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('outflow_station|outflow_datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('output2file_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('output_filename','100','Optional',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('override_destination_validation_requirement_yn',null,'Normally, if any of the destination measurements are validated, then this process will not be performed. Setting this flag to Yes will force the process to be performed even if some destination measurements are validated.',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('override_source_validation_requirement_yn',null,'Normally, if any of the source measurements are not validated, then this process will not be performed. Setting this flag to Yes will force the process to be performed even if some source measurements are not validated.',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('paid_amount_due_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('payment_amount','10',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('payroll_year','4',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('period_number','2',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('plot_for_station_check_yn','1','Create a plot to check station''s performance',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('positive_station|positive_datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('really_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('revoke_only_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('shef_filename','100','For server files, use backslash for directories','y',null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('source_directory','100',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('source_station|source_datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('stage_units_converted',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('starting_check_number','5',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('station|datatype',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('summary_only_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('symbols_yn',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('system_folders_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('transaction_amount','10','Optionally override the default.',null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('transaction_date','10',null,null,'y');
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('transmit_validation_info_yn',null,null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('undo_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('utm_easting','6',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('utm_northing','7',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('where_clause','100',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('with_file_trim_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('with_stub_yn','1',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('x_tick_major','6',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('x_tick_minor','6',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('ymax','6',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('ymin','6',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('y_tick_major','6',null,null,null);
insert into prompt (prompt,input_width,hint_message,upload_filename_yn,date_yn) values ('y_tick_minor','6',null,null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('aggregation',null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('database_management_system','Defaults to Mysql',null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('execute_select_statement_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('export_output',null,null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('finance_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('group_annually_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('inventory_balance_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('load_srf_output_medium',null,'output_medium');
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('null','Necessary for outter joins to work',null);
insert into drop_down_prompt (drop_down_prompt,hint_message,optional_display) values ('output_medium',null,null);
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregation','consolidated','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('aggregation','sequential','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('database_management_system','mysql','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('database_management_system','oracle','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('execute_select_statement_output_medium','spreadsheet','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('execute_select_statement_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('export_output','gzip_file','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('export_output','shell_script','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','PDF','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('finance_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('group_annually_output_medium','chart','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('group_annually_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('group_annually_output_medium','text_file','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('inventory_balance_output_medium','PDF','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('inventory_balance_output_medium','table','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('load_srf_output_medium','audit_report','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('load_srf_output_medium','database','3');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('load_srf_output_medium','screen','1');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','table','2');
insert into drop_down_prompt_data (drop_down_prompt,drop_down_prompt_data,display_order) values ('output_medium','text_file','3');
insert into operation (operation,output_yn) values ('delete','n');
insert into operation (operation,output_yn) values ('delete_isa_only',null);
insert into operation (operation,output_yn) values ('detail','y');
insert into operation (operation,output_yn) values ('google_map','y');
insert into operation (operation,output_yn) values ('ledger_propagate','n');
insert into operation (operation,output_yn) values ('null','y');
insert into process_groups (process_group) values ('documentation');
insert into process_groups (process_group) values ('load');
insert into process_groups (process_group) values ('manipulate');
insert into process_groups (process_group) values ('output');
insert into process_groups (process_group) values ('shortcut');
all_done3
) | sql.e 2>&1 | grep -vi duplicate


(
cat << all_done4
insert into self (full_name,
		street_address,
		inventory_cost_method,
		payroll_pay_period,
		payroll_beginning_day,
		social_security_combined_tax_rate,
		social_security_payroll_ceiling,
		medicare_combined_tax_rate,
		medicare_additional_withholding_rate,
		medicare_additional_gross_pay_floor,
		federal_withholding_allowance_period_value,
		federal_nonresident_withholding_income_premium,
		state_withholding_allowance_period_value,
		state_itemized_allowance_period_value,
		federal_unemployment_wage_base,
		federal_unemployment_tax_standard_rate,
		federal_unemployment_threshold_rate,
		federal_unemployment_tax_minimum_rate,
		state_unemployment_wage_base,
		state_unemployment_tax_rate,
		state_sales_tax_rate) values ('changeme','1234 Main St.','LIFO','weekly','friday','0.1240','127200','0.0290',null,null,'77.90',null,null,'19.50','7000','0.0600','0.0540','0.0060','7000','0.0340','0.0850');
insert into entity (full_name,street_address) values ('changeme','1234 Main St.');
all_done4
) | sql.e 2>&1 | grep -vi duplicate

exit 0
