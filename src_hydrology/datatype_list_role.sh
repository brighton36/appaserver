:
# datatype_list_role.sh
# ---------------------
# This shell script originally was created to omit the
# secondary datatypes from the public users. However, because
# of a contradiction, this script has been reverted back
# to performing simply the function of selecting every datatype.
# -------------------------------------------------------------- 

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application role" 1>&2
	exit 1
fi

application=$1
role_name=$2

datatype_table=`get_table_name $application datatype`
#station_datatype_table=`get_table_name $application station_datatype`
#role_table=`get_table_name $application role`

#override_row_restrictions_yn=`						 \
#	echo "	select override_row_restrictions_yn			 \
#		from $role_table					 \
#		where $role_table.role = '$role_name';"			|\
#	sql.e`
#
#if [ "$override_row_restrictions_yn" = 'y' ]
#then
#	validation_required_filter="1 = 1"
#else
#	validation_required_filter="validation_required_yn = 'y'"
#fi

#if [ "$role_name" = public ]
#then
#	validation_required_filter="validation_required_yn = 'y'"
#else
	validation_required_filter="1 = 1"
#fi

#echo "	select distinct datatype 					\
#	from $station_datatype_table					\
#	where $validation_required_filter				\
#	order by datatype;"						|
#sql.e									|
#cat

echo "	select datatype 						\
	from $datatype_table						\
	order by datatype;"						|
sql.e									|
cat

exit 0
