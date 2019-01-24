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
delete from $folder where folder = 'feeder_account';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,notepad,post_change_process,post_change_javascript,html_help_file_anchor,no_initial_capital_yn,exclude_application_export_yn,appaserver_yn,lookup_email_output_yn,data_directory,index_directory,create_view_statement) values ('feeder_account','table','5','bank_upload',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'feeder_account';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_event','feeder_account','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_feeder_archive','feeder_account','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'feeder_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('feeder_account','text','40',null,null,'External accounts like credit card and Pay Pal that feed in transactions.',null,null,null,null);
delete from $folder_attribute where folder = 'feeder_account';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('feeder_account','feeder_account','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('feeder_account','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('feeder_account','supervisor','update');
delete from $javascript_folders where folder = 'feeder_account';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('bank_upload');
insert into $role_operation (folder,role,operation) values ('feeder_account','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('feeder_account','supervisor','detail');
delete from $folder where folder = 'bank_upload_event';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,notepad,post_change_process,post_change_javascript,html_help_file_anchor,no_initial_capital_yn,exclude_application_export_yn,appaserver_yn,lookup_email_output_yn,data_directory,index_directory,create_view_statement) values ('bank_upload_event','prompt','5','bank_upload','n',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_event';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_event','appaserver_user','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_event','feeder_account','null',null,null,null,null,null,null,null,null,null,null,null);
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
delete from $attribute where attribute = 'feeder_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('feeder_account','text','40',null,null,'External accounts like credit card and Pay Pal that feed in transactions.',null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_event';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_date_time','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','bank_upload_filename',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','completed_date_time',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','feeder_account',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','file_sha256sum',null,'4',null,null,'y',null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_event','login_name',null,'1','y',null,null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_event','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_event','supervisor','lookup');
delete from $javascript_folders where folder = 'bank_upload_event';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('bank_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('bank_upload_event','supervisor','detail');
delete from $folder where folder = 'bank_upload_feeder_archive';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,notepad,post_change_process,post_change_javascript,html_help_file_anchor,no_initial_capital_yn,exclude_application_export_yn,appaserver_yn,lookup_email_output_yn,data_directory,index_directory,create_view_statement) values ('bank_upload_feeder_archive','prompt','5','bank_upload',null,null,null,null,null,null,null,null,null,null,null,null,'create view bank_upload_feeder_archive as select bank_date, bank_description, feeder_account, sequence_number, bank_amount, bank_running_balance, bank_upload_event.bank_upload_date_time from bank_upload_event, bank_upload_archive where bank_upload_event.bank_upload_date_time = bank_upload_archive.bank_upload_date_time;');
delete from $relation where folder = 'bank_upload_feeder_archive';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_feeder_archive','feeder_account','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_description','text','140',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'feeder_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('feeder_account','text','40',null,null,'External accounts like credit card and Pay Pal that feed in transactions.',null,null,null,null);
delete from $attribute where attribute = 'sequence_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sequence_number','integer','9',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_amount','float','10','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_running_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_running_balance','float','12','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'bank_upload_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_upload_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_feeder_archive';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_feeder_archive','bank_amount',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_feeder_archive','bank_date','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_feeder_archive','bank_description','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_feeder_archive','bank_running_balance',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_feeder_archive','bank_upload_date_time',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_feeder_archive','feeder_account',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_feeder_archive','sequence_number',null,'2',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_feeder_archive','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_feeder_archive','supervisor','lookup');
delete from $javascript_folders where folder = 'bank_upload_feeder_archive';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('bank_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_feeder_archive','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_feeder_archive','supervisor','detail');
delete from $folder where folder = 'bank_upload_transaction_balance';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,notepad,post_change_process,post_change_javascript,html_help_file_anchor,no_initial_capital_yn,exclude_application_export_yn,appaserver_yn,lookup_email_output_yn,data_directory,index_directory,create_view_statement) values ('bank_upload_transaction_balance','prompt','5','bank_upload',null,null,null,null,null,null,null,null,null,null,null,null,'create view bank_upload_transaction_balance as select transaction.transaction_date_time ,bank_upload. bank_date, bank_upload.bank_description, transaction.full_name , transaction.street_address, transaction_amount, bank_amount, journal_ledger.balance, bank_running_balance, sequence_number from bank_upload_transaction, transaction, bank_upload, journal_ledger, account where bank_upload_transaction.full_name = transaction.full_name and bank_upload_transaction.street_address = transaction.street_address and bank_upload_transaction.transaction_date_time = transaction.transaction_date_time and bank_upload_transaction.bank_date = bank_upload.bank_date and bank_upload_transaction.bank_description = bank_upload.bank_description and transaction.full_name = journal_ledger.full_name and transaction.street_address = journal_ledger.street_address and transaction.transaction_date_time = journal_ledger.transaction_date_time and journal_ledger.account = account.account and account.hard_coded_account_key = ''cash_key'';');
delete from $relation where folder = 'bank_upload_transaction_balance';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_transaction_balance','bank_upload','null',null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_transaction_balance','bank_upload_transaction','null',null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_date','date','10',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_description';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_description','text','140',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_amount','float','10','2',null,null,null,null,null,null);
delete from $attribute where attribute = 'bank_amount';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_amount','float','10','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('balance','float','11','2',null,null,null,'y','y',null);
delete from $attribute where attribute = 'bank_running_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('bank_running_balance','float','12','2',null,null,null,null,null,'n');
delete from $attribute where attribute = 'sequence_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('sequence_number','integer','9',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'bank_upload_transaction_balance';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','balance',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','bank_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','bank_date','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','bank_description','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','bank_running_balance',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','full_name','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','sequence_number',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','street_address','5',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','transaction_amount',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_transaction_balance','transaction_date_time','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_transaction_balance','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_transaction_balance','supervisor','lookup');
delete from $javascript_folders where folder = 'bank_upload_transaction_balance';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('bank_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_transaction_balance','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_transaction_balance','supervisor','detail');
delete from $folder where folder = 'bank_upload_archive';
insert into $folder (folder,form,insert_rows_number,subschema,lookup_before_drop_down_yn,populate_drop_down_process,notepad,post_change_process,post_change_javascript,html_help_file_anchor,no_initial_capital_yn,exclude_application_export_yn,appaserver_yn,lookup_email_output_yn,data_directory,index_directory,create_view_statement) values ('bank_upload_archive','prompt','5','bank_upload',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'bank_upload_archive';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('bank_upload_archive','bank_upload_event','null',null,null,null,null,null,null,null,null,null,null,null);
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
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_amount',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_date','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_description','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_running_balance',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','bank_upload_date_time',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('bank_upload_archive','sequence_number',null,'1',null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('bank_upload_archive','bookkeeper','lookup');
insert into $role_folder (folder,role,permission) values ('bank_upload_archive','supervisor','lookup');
delete from $javascript_folders where folder = 'bank_upload_archive';
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('bookkeeper','y',null);
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschemas (subschema) values ('bank_upload');
insert into $role_operation (folder,role,operation) values ('bank_upload_archive','bookkeeper','detail');
insert into $role_operation (folder,role,operation) values ('bank_upload_archive','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
