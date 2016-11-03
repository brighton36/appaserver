:
# src_everglades/grace_graph_each_station.sh
# ------------------------------------------

if [ "$#" -ne 3 ]
then
        echo \
"Usage: $0 application destination_pdf_directory number_days_ago_to_include"
1>&2
        exit 1
fi

application=$1
destination_directory=$2
days_ago=$3

PATH=.:/opt/html/cgi:/opt/html/utility:/opt/html/bin:$PATH; export PATH
PATH=/opt/html/src_appaserver:$PATH; export PATH
execution_directory=/opt/html/cgi; cd $execution_directory

begin_date=`date.e -$days_ago`
end_date=`date.e -1`

# Session 888 is a special session to force cycle colors.
# -------------------------------------------------------
session=888
#session=999
plot_for_station_check_yn=y

# As defined in library/grace.h
# -----------------------------
pdf_file=../$application/chart_$session.pdf
agr_file=../$application/chart_$session.agr

for station in `station_list.sh everglades data_man`
do
        custom_chart            $application            		\
				role					\
                                $session                		\
                                $station                		\
                                datatype                		\
                                $begin_date             		\
                                $end_date               		\
                                real_time               		\
                                ymin                    		\
                                ymax                    		\
                                y_tick_major            		\
                                y_tick_minor            		\
                                gridlines_yn            		\
                                cycle_color_upon_time_series_break_yn   \
                                cycle_color_upon_null_value_yn  	\
                                email_address           		\
                                validation_level        		\
                                aggregate_statistic     		\
                                symbols_yn				\
				$plot_for_station_check_yn

        mv $pdf_file $destination_directory/$station.pdf
        mv $agr_file $destination_directory/$station.agr
done
