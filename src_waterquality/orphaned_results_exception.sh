:
echo "select station, collection_date, collection_time, parameter, units from results_exception where not exists ( select 1 from results where results_exception.station = results.station and results_exception.collection_date = results.collection_date and results_exception.collection_time = results.collection_time and results_exception.parameter = results.parameter and results_exception.units = results.units );"	|
sql.e

exit 0
