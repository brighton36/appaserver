:
get_folder_data	application=modeland			\
		select=date,value		 	\
		folder=model_results		 	\
		where=" model_results.station in ('BBSM_10023') and model_results.model in ('BBSM') and model_results.version in ('2') and model_results.alternative in ('Base') and model_results.datatype in ('salinity') and date >= '2000-1-1' and date <= '2005-1-31'"				|
sort -r							|
real_time2aggregate_value.e aggregate_statistic 0 -1 1 '^' weekly n 2005-1-31								|
cat
