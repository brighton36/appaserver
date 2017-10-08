#!/bin/sh
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

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
delete from $folder where folder = 'foreign_attribute';
insert into $folder (folder,form,insert_rows_number,subschema,appaserver_yn,lookup_before_drop_down_yn,populate_drop_down_process,notepad,html_help_file_anchor,no_initial_capital_yn,post_change_javascript,post_change_process,exclude_application_export_yn,lookup_email_output_yn,data_directory,index_directory) values ('foreign_attribute','prompt','5','folder','y',null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'foreign_attribute';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('foreign_attribute','folder_attribute','foreign_attribute',null,null,null,null,null,null,null,null,null,null);
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('foreign_attribute','relation','null',null,null,null,null,null,null,null,null,null,null);
delete from $attribute where attribute = 'folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('folder','text','35',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'related_folder';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('related_folder','text','35',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'related_attribute';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('related_attribute','text','60',null,'It is required that all%0D%0Aforeign keys be the same name%0D%0Aas the corresponding primary%0D%0Akeys, except the last one. If%0D%0Athe last foreign key differs%0D%0Afrom the last primary key, set%0D%0Athis attribute to it. This is%0D%0Anecessary for recursive or%0D%0Amultiple relationships. For%0D%0Aexample, the table RELATION%0D%0Ahas two relationships to%0D%0AFOLDER.',null,null,null,null,'y');
delete from $attribute where attribute = 'foreign_attribute';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('foreign_attribute','text','60',null,null,null,null,null,null,'y');
delete from $attribute where attribute = 'primary_key_index';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,hint_message,post_change_javascript,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('primary_key_index','integer','3',null,null,null,null,null,null,'y');
delete from $folder_attribute where folder = 'foreign_attribute';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('foreign_attribute','folder','1',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('foreign_attribute','foreign_attribute','4',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('foreign_attribute','primary_key_index',null,'1',null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('foreign_attribute','related_attribute','3',null,null,null,null,null,null,null,null);
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_prompt_yn,omit_insert_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('foreign_attribute','related_folder','2',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('foreign_attribute','system','insert');
insert into $role_folder (folder,role,permission) values ('foreign_attribute','system','update');
delete from $javascript_folders where folder = 'foreign_attribute';
insert into $subschemas (subschema) values ('folder');
insert into $role_operation (folder,role,operation) values ('foreign_attribute','system','delete');
insert into $role_operation (folder,role,operation) values ('foreign_attribute','system','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

exit 0
