#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/export_predictivebooks.sh
# -----------------------------------------------------------------
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application input_file cleanup_script" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

input_file=$2
cleanup_script=$3

directory=$appaserver_home/src_predictive
output_shell="`basename.e $input_file y`".sh

source_file="$APPASERVER_HOME/library/appaserver_library.h"

echo "Here is $source_file"
grep DIFFERENT_DESTINATION_APPLICATION_OK $source_file
echo "sleeping..."
sleep 5
echo "executing..."

appaserver_config_file="/etc/appaserver.config"

label="appaserver_mount_point="
appaserver_home=`cat $appaserver_config_file	| \
		 grep "^${label}"		| \
		 sed "s/$label//"`

function export_processes()
{
	application=$1
	input_file=$2
	output_shell=$3

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done3" >> $output_shell

	# role_operation
	# --------------
	cat $input_file							|
	while read folder
	do
		where="folder = '$folder'"
		select="folder,role,operation"
		echo "select $select from role_operation where $where;"	|
		sql.e							|
		insert_statement.e t=role_operation f="$select" del='^'	|
		cat >> $output_shell
	done

	# process
	# -------
	select="process,
		command_line,
		notepad,
		html_help_file_anchor,
		post_change_javascript,
		process_set_display,
		process_group,
		preprompt_help_text,
		appaserver_yn"
	echo "select "$select" from process;"			|
	sql.e							|
	insert_statement.e t=process f="$select" del='^'	|
	cat >> $output_shell

	# process_parameter
	# -----------------
	select="process,
		folder,
		attribute,
		drop_down_prompt,
		prompt,
		display_order,
		drop_down_multi_select_yn,
		preprompt_yn,
		populate_drop_down_process,
		populate_helper_process"
	echo "select "$select" from process_parameter;"			|
	sql.e								|
	insert_statement.e t=process_parameter f="$select" del='^'	|
	cat >> $output_shell

	# role_process
	# ------------
	select="process,role"
	echo "select "$select" from role_process;"			|
	sql.e								|
	insert_statement.e t=role_process f="$select" del='^'		|
	cat >> $output_shell

	# javascript_processes
	# --------------------
	select="process,javascript_filename"
	echo "select "$select" from javascript_processes;"		|
	sql.e								|
	insert_statement.e t=javascript_processes f="$select" del='^'	|
	cat >> $output_shell

	# javascript_files
	# ----------------
	select="javascript_filename"
	echo "select "$select" from javascript_files;"			|
	sql.e								|
	insert_statement.e t=javascript_files f="$select" del='^'	|
	cat >> $output_shell

	# prompt
	# ------
	select="prompt,input_width,hint_message,upload_filename_yn,date_yn"
	echo "select "$select" from prompt;"				|
	sql.e								|
	insert_statement.e t=prompt f="$select" del='^'			|
	cat >> $output_shell

	# drop_down_prompt
	# ----------------
	select="drop_down_prompt,hint_message,optional_display"
	echo "select "$select" from drop_down_prompt;"			|
	sql.e								|
	insert_statement.e t=drop_down_prompt f="$select" del='^'	|
	cat >> $output_shell

	# drop_down_prompt_data
	# ---------------------
	select="drop_down_prompt,drop_down_prompt_data,display_order"
	echo "select "$select" from drop_down_prompt_data;"		|
	sql.e								|
	insert_statement.e t=drop_down_prompt_data f="$select" del='^'	|
	cat >> $output_shell

	# operation
	# ---------
	select="operation,output_yn"
	echo "select $select from operation;"			|
	sql.e							|
	insert_statement.e t=operation f="$select" del='^'	|
	cat >> $output_shell

	# process_groups
	# --------------
	select="process_group"
	echo "select $select from process_groups;"		|
	sql.e							|
	insert_statement.e t=process_groups f="$select" del='^'	|
	cat >> $output_shell

	echo "all_done3" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# export_processes()

function create_predictivebooks()
{
	application=$1
	input_file=$2
	output_shell=$3

	command="create_table $application n x x x $application ttable n mysql"

	cat $input_file							|
	while read table
	do
		new_command=`echo $command | sed "s/ttable/$table/"`
		create_file=`
		$new_command						|
		grep Created						|
		column.e 1						|
		cat`

		cat $create_file					|
		egrep "^echo|^table_name="				|
		grep -v 'drop table'					|
		cat >> $output_shell
	done
}
# create_predictivebooks()

function export_predictivebooks()
{
	application=$1
	input_file=$2
	output_shell=$3

	parameter_list=`
	cat $input_file							|
	count.e 1 							|
	sed 's/ //'							|
	sed 's/)/=/'							|
	sed 's/^00*//'							|
	sed 's/^/folder_/'						|
	joinlines.e '&'							|
	cat`

	export_file=`
	export_subschema $application x x x x "${parameter_list}" 2>/dev/null |
	grep Created							      |
	column.e 1							      |
	cat`

	cat $export_file						|
	sed "s/!= ${application}/= ignored/"				|
	grep -v '^exit'							|
	cat >> $output_shell
}
# export_predictivebooks()

function extract_investment()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done5" >> $output_shell

	folder=investment_classification
	columns=classification
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e table=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=investment_operation
	columns=investment_operation
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	echo "all_done5" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_investment()

function extract_chart_of_accounts()
{
	application=$1
	input_file=$2
	output_shell=$3

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done2" >> $output_shell

	folder=element
	columns=element,accumulate_debit_yn
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e table=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=subclassification
	columns=subclassification,element,display_order
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=account
	columns=account,subclassification,hard_coded_account_key
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=depreciation_method
	columns=depreciation_method
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=day
	columns=day
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=tax_form_line_account
	columns=tax_form,tax_form_line,account
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=tax_form_line
	columns="	tax_form,
			tax_form_line,
			tax_form_description,
			itemize_accounts_yn"
	get_folder_data a=$application f=$folder s="$columns"		|
	insert_statement.e t=$folder field="$columns" del='^'		|
	cat >> $output_shell

	folder=tax_form
	columns=tax_form
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	# Only professional folders follow:
	# ---------------------------------
	folder=payroll_pay_period
	results=`grep $folder $input_file | wc -l`
	if [ "$results" -eq 1 ]
	then
		columns=payroll_pay_period
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	
		folder=inventory_cost_method
		columns=inventory_cost_method
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	
		folder=federal_marital_status
		columns=federal_marital_status
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	
		folder=state_marital_status
		columns=state_marital_status
		get_folder_data a=$application f=$folder s=$columns	|
		insert_statement.e t=$folder field=$columns del='^'	|
		cat >> $output_shell
	
		folder=federal_income_tax_withholding
		columns="federal_marital_status,
			 income_over,
			 income_not_over,
			 tax_fixed_amount,
			 tax_percentage_amount"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=state_income_tax_withholding
		columns="state_marital_status,
			 income_over,
			 income_not_over,
			 tax_fixed_amount,
			 tax_percentage_amount"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=state_standard_deduction_table
		columns="state_marital_status,
			 state_withholding_allowances,
			 state_standard_deduction_amount"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=fixed_service_category
		columns="service_category"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	
		folder=hourly_service_category
		columns="service_category"
		get_folder_data a=$application f=$folder s="$columns"	|
		insert_statement.e t=$folder field="$columns" del='^'	|
		cat >> $output_shell
	fi

	echo "all_done2" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_chart_of_accounts()

function extract_self()
{
	application=$1
	output_shell=$2

	full_name="changeme"
	street_address="1234 Main St."

	echo "" >> $output_shell
	echo "(" >> $output_shell
	echo "cat << all_done4" >> $output_shell

	folder=self
	select="'$full_name',
		'$street_address',
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
		state_sales_tax_rate"
	fields="full_name,
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
		state_sales_tax_rate"
	get_folder_data a=$application f=$folder s="$select"		|
	insert_statement.e table=$folder field="$fields" del='^'	|
	cat >> $output_shell

	folder=entity
	fields="full_name,street_address"
	echo "$full_name^$street_address"				|
	insert_statement.e table=$folder field=$fields del='^'		|
	cat >> $output_shell

	echo "all_done4" >> $output_shell
	echo ") | sql.e 2>&1 | grep -vi duplicate" >> $output_shell
	echo "" >> $output_shell
}
# extract_self()

rm $output_shell 2>/dev/null

export_predictivebooks $application $input_file $output_shell
create_predictivebooks $application $input_file $output_shell
extract_chart_of_accounts $application $input_file $output_shell
extract_investment $application $output_shell
export_processes $application $input_file $output_shell
extract_self $application $output_shell

echo "exit 0" >> $output_shell

if [ "$cleanup_script" != "" -a "$cleanup_script" != "cleanup_script" ]
then
	$cleanup_script $output_shell
fi

chmod +x $output_shell

echo Created $output_shell

exit 0
