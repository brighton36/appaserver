:
# /opt/html/src_hydrology/interpolate_spike_update.sh
# ---------------------------------------------------

if [ "$#" -ne 5 ]
then
	echo \
"Usage: $0 entity application login_name minimum_spike really_yn" 1>&2
	exit 1
fi

entity=$1
application=$2
login_name=$3
minimum_spike=$4
really_yn=$5

table_name=`get_table_name $entity $application measurement`

if [ "$really_yn" = "y" ]
then
	update_process=sql.e
else
	update_process="./nothing"
fi

interpolate_spike_update 						  \
     entity=$entity							  \
     application=$application						  \
     login_name=$login_name						  \
     primary_key_delimiter=','						  \
     value_delimiter='|'						  \
     maximum_out_of_range=7						  \
     minimum_spike=$minimum_spike			  		  \
     primary_key_name_comma_list=station,datatype,measurement_date,measurement_time \
     table_name=$table_name						  \
     really_yn=$really_yn						  \
     value_column_name=measurement_value				  |
$update_process
