#!/bin/sh
# ---------------------------------------------
# parameter_records4process.sh
# ---------------------------------------------
#
# Note: no outer join is necessary because
#       null records exist in tables
#       prompt and drop_down_prompt.
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

#echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
"Usage: $0 application process is_preprompt_yn" 1>&2
	exit 1
fi

application=$1
process=$2
is_preprompt_yn=$3

process_parameter=`get_table_name $application process_parameter`
prompt=`get_table_name $application prompt`
drop_down_prompt=`get_table_name $application drop_down_prompt`

if [ "$is_preprompt_yn" = "y" ]
then
	preprompt_where="preprompt_yn = 'y'"
else
	preprompt_where=\
"(preprompt_yn is null or preprompt_yn != 'y')"
fi

echo "select folder,							\
	     attribute,							\
	     $process_parameter.prompt,					\
	     $process_parameter.drop_down_prompt,			\
	     $prompt.input_width,					\
	     $prompt.hint_message,					\
	     $drop_down_prompt.hint_message,				\
	     $drop_down_prompt.optional_display,			\
	     $process_parameter.drop_down_multi_select_yn,		\
	     $process_parameter.populate_drop_down_process,		\
	     $prompt.upload_filename_yn,				\
	     $prompt.date_yn,						\
	     $process_parameter.populate_helper_process			\
      from $process_parameter, $prompt, $drop_down_prompt		\
      where $process_parameter.process = '$process'			\
	and $process_parameter.prompt = $prompt.prompt			\
	and $process_parameter.drop_down_prompt =			\
		$drop_down_prompt.drop_down_prompt			\
	and $preprompt_where						\
      order by display_order;"						|
sql.e '^'

