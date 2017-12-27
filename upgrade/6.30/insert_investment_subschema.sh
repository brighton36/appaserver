#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != timriley -a		\
     "$application" != eves -a			\
     "$application" != capitolpops -a		\
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
delete from $folder where folder = 'equity_account_balance';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,notepad,post_change_javascript,html_help_file_anchor,post_change_process,no_initial_capital_yn,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory) values ('equity_account_balance','prompt','5',null,null,null,null,null,'post_change_account_balance',null,null,null,null,null,'investment',null);
delete from $relation where folder = 'equity_account_balance';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('equity_account_balance','investment_account','null',null,null,'y',null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('equity_account_balance','investment_operation','null',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('equity_account_balance','transaction','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account_number','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'investment_operation';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('investment_operation','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'transaction_date_time';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('transaction_date_time','current_date_time','19',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'share_price';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('share_price','float','12','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'share_quantity_change';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('share_quantity_change','float','12','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'share_quantity_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('share_quantity_balance','float','12','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'book_value_change';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('book_value_change','float','12','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'book_value_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('book_value_balance','float','13','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'moving_share_price';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('moving_share_price','float','13','5',null,null,null,'y','y',null);
delete from $attribute where attribute = 'total_cost_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('total_cost_balance','float','13','5',null,null,null,null,null,null);
delete from $attribute where attribute = 'market_value';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('market_value','float','13','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'unrealized_gain_balance';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('unrealized_gain_balance','float','12','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'unrealized_gain_change';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('unrealized_gain_change','float','12','4',null,null,null,'y','y',null);
delete from $attribute where attribute = 'realized_gain';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('realized_gain','float','12','4',null,null,null,null,null,null);
delete from $folder_attribute where folder = 'equity_account_balance';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','account_number','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','book_value_balance',null,'9','y',null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','book_value_change',null,'8','y',null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','date_time','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','investment_operation',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','market_value',null,'12',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','moving_share_price',null,'10','y',null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','realized_gain',null,'15','y',null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','share_price',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','share_quantity_balance',null,'7',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','share_quantity_change',null,'6',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','street_address','2',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','total_cost_balance',null,'11','y',null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','transaction_date_time',null,'4','y',null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','unrealized_gain_balance',null,'13','y',null,null,null,'y',null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('equity_account_balance','unrealized_gain_change',null,'14','y',null,null,null,'y',null,null);
insert into $role_folder (folder,role,permission) values ('equity_account_balance','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('equity_account_balance','supervisor','update');
delete from $javascript_folders where folder = 'equity_account_balance';
insert into $subschemas (subschema) values ('investment');
insert into $role_operation (folder,role,operation) values ('equity_account_balance','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('equity_account_balance','supervisor','detail');
delete from $folder where folder = 'financial_institution';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,notepad,post_change_javascript,html_help_file_anchor,post_change_process,no_initial_capital_yn,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory) values ('financial_institution','prompt','5',null,null,null,null,null,null,null,null,null,null,null,'entity',null);
delete from $relation where folder = 'financial_institution';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('financial_institution','entity','null',null,null,null,'y',null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('investment_account','financial_institution','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'financial_institution';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('financial_institution','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('financial_institution','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('financial_institution','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('financial_institution','supervisor','update');
delete from $javascript_folders where folder = 'financial_institution';
insert into $subschemas (subschema) values ('entity');
insert into $role_operation (folder,role,operation) values ('financial_institution','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('financial_institution','supervisor','detail');
delete from $folder where folder = 'investment_account';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,notepad,post_change_javascript,html_help_file_anchor,post_change_process,no_initial_capital_yn,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory) values ('investment_account','prompt','5',null,null,null,null,null,null,null,null,'y',null,null,'investment',null);
delete from $relation where folder = 'investment_account';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('investment_account','duration_term','null',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('investment_account','financial_institution','null',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('investment_account','investment_classification','null',null,null,null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('equity_account_balance','investment_account','null',null,null,'y',null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'full_name';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('full_name','text','60',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'street_address';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('street_address','text','40',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'account_number';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('account_number','text','50',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'classification';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('classification','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'duration_term';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('duration_term','text','15',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'certificate_maturity_months';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('certificate_maturity_months','integer','3',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'certificate_maturity_date';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('certificate_maturity_date','date','11',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'interest_rate';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('interest_rate','float','5','2',null,'0.0 rate < 10.0',null,null,null,null);
delete from $folder_attribute where folder = 'investment_account';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','account_number','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','certificate_maturity_date',null,'4',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','certificate_maturity_months',null,'3',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','classification',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','duration_term',null,'2',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','full_name','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','interest_rate',null,'5',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_account','street_address','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('investment_account','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('investment_account','supervisor','update');
delete from $javascript_folders where folder = 'investment_account';
insert into $subschemas (subschema) values ('investment');
insert into $role_operation (folder,role,operation) values ('investment_account','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('investment_account','supervisor','detail');
delete from $folder where folder = 'investment_classification';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,notepad,post_change_javascript,html_help_file_anchor,post_change_process,no_initial_capital_yn,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory) values ('investment_classification','table','5',null,null,null,null,null,null,null,null,null,null,null,'static',null);
delete from $relation where folder = 'investment_classification';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('investment_account','investment_classification','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'classification';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('classification','text','15',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'investment_classification';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_classification','classification','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('investment_classification','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('investment_classification','supervisor','update');
delete from $javascript_folders where folder = 'investment_classification';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('investment_classification','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('investment_classification','supervisor','detail');
delete from $folder where folder = 'investment_operation';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,notepad,post_change_javascript,html_help_file_anchor,post_change_process,no_initial_capital_yn,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory) values ('investment_operation','table','5',null,null,null,null,null,null,null,null,null,null,null,'static',null);
delete from $relation where folder = 'investment_operation';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('equity_account_balance','investment_operation','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'investment_operation';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('investment_operation','text','15',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'investment_operation';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('investment_operation','investment_operation','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('investment_operation','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('investment_operation','supervisor','update');
delete from $javascript_folders where folder = 'investment_operation';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('investment_operation','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('investment_operation','supervisor','detail');
delete from $folder where folder = 'duration_term';
insert into $folder (folder,form,insert_rows_number,populate_drop_down_process,lookup_email_output_yn,notepad,post_change_javascript,html_help_file_anchor,post_change_process,no_initial_capital_yn,exclude_application_export_yn,lookup_before_drop_down_yn,appaserver_yn,data_directory,subschema,index_directory) values ('duration_term','prompt','5',null,null,null,null,null,null,null,null,null,null,null,'static',null);
delete from $relation where folder = 'duration_term';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,omit_mto1_join_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('investment_account','duration_term','null',null,null,null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'duration_term';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('duration_term','text','15',null,null,null,null,null,null,null);
delete from $folder_attribute where folder = 'duration_term';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('duration_term','duration_term','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('duration_term','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('duration_term','supervisor','update');
delete from $javascript_folders where folder = 'duration_term';
insert into $subschemas (subschema) values ('static');
insert into $role_operation (folder,role,operation) values ('duration_term','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('duration_term','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
