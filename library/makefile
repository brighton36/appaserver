# library/makefile
# -------------------------

CC=gcc
#CFLAGS=-g
#WARN = -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall -lctemplate_nothreads ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library
UTILITY=${APPASERVER_HOME}/utility

STANDARD_LIBRARY=						\
		date.o 						\
		julian.o 					\
		dictionary.o 					\
		list.o 						\
		list_usage.o 					\
		timlib.o 					\
		piece.o 					\
		column.o 					\
		date_convert.o 					\
		hashtbl.o

APPASERVER_LIBRARY=						\
		aggregate_level.o 				\
		aggregate_statistic.o				\
		decode_html_post.o				\
		update_statement.o				\
		fopen_path.o					\
		name_arg.o					\
		html_table.o					\
		google_earth.o					\
		google_chart.o					\
		environ.o 					\
		html_invoice.o					\
		latex.o						\
		latex_invoice.o					\
		basename.o					\
		role.o						\
		role_folder.o					\
		make_date_time_between_compatable.o		\
		populate_helper_process.o			\
		lookup_before_drop_down.o			\
		appaserver_library.o				\
		appaserver_error.o				\
		appaserver.o					\
		folder.o					\
		attribute.o					\
		date_aggregate.o				\
		process_generic_output.o			\
		horizontal_menu.o				\
		folder_menu.o					\
		application.o					\
		appaserver_user.o				\
		element.o					\
		sed.o						\
		form.o						\
		query.o 					\
		stack.o 					\
		grace.o 					\
		frameset.o					\
		validation_level.o				\
		related_folder.o				\
		operation_list.o				\
		operation.o					\
		document.o					\
		insert_database.o				\
		expected_count.o				\
		update_database.o				\
		delete_database.o				\
		process_parameter_list.o			\
		start_stop_time.o				\
		appaserver_parameter_file.o			\
		prompt_recursive.o				\
		appaserver_link_file.o				\
		process.o					\
		queue.o						\
		validation_form.o				\
		post2dictionary.o				\
		period_wo_date.o				\
		dictionary2file.o				\
		application_constants.o				\
		query_attribute_statistics_list.o		\
		table.o						\
		row_security.o					\
		dictionary_appaserver.o				\
		additional_drop_down_attribute.o		\
		scatterplot.o					\
		regression.o					\
		pivot_table.o					\
		generic_load.o					\
		statistics_weighted.o				\
		report_writer.o					\
		semaphore.o					\
		dollar.o					\
		array.o						\
		session.o					\
		mysqldump.o

all: ${APPASERVER_LIBRARY} ${STANDARD_LIBRARY}				\
		distance.o						\
		waypoint.o						\
		post_login_library.o					\
		covariance_dvr.e					\
		semaphore_dvr.e						\
		dollar_dvr.e						\
		fork_control_dvr.e					\
		queue_dvr.e						\
		hashdvr.e						\
		julian_dvr.e						\
		moving_average_dvr.e					\
		histogram_dvr.e						\
		sed_dvr.e						\
		queue_dvr.e						\
		easycharts.o						\
		pair_one2m.o						\
		keys_match_sum.o					\
		julian_time_double_list.o				\
		token_dvr.e \
		ctemplate_wrapper.o

touch:
	/usr/bin/touch *.c

clean:
	rm -f *.[oe]

fork_control_dvr.e: fork_control_dvr.c fork_control.o
	${CC} ${CFLAGS} -I${LIBRARY} fork_control_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		fork_control.o						\
	   -o fork_control_dvr.e

semaphore_dvr.e: semaphore_dvr.c semaphore.o
	${CC} ${CFLAGS} -I${LIBRARY} semaphore_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		semaphore.o						\
	   -o semaphore_dvr.e

dollar_dvr.e: dollar.c dollar.o dollar_dvr.c
	${CC} ${CFLAGS} -I${LIBRARY} dollar_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		dollar.o						\
	   -o dollar_dvr.e

covariance_dvr.e: covariance_dvr.c covariance.o
	${CC} ${CFLAGS} -I${LIBRARY} covariance_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		covariance.o						\
	   -o covariance_dvr.e

row_security_dvr.e: row_security_dvr.c					\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}
	${CC} ${CFLAGS} -I${LIBRARY}					\
		row_security_dvr.c					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${APPASERVER_LIBRARY}					\
		${STANDARD_LIBRARY}					\
	   -o row_security_dvr.e

fifo_dvr.e: fifo_dvr.c fifo.o
	${CC} ${CFLAGS} -I${LIBRARY} fifo_dvr.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		fifo.o							\
	   -o fifo_dvr.e

pipe_dvr.e: pipe_dvr.c pipe.o
	${CC} ${CFLAGS} -I${LIBRARY} pipe_dvr.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		pipe.o							\
	   -o pipe_dvr.e

google_map_dvr.e: google_map_dvr.c google_map.o environ.h application.h	\
		${STANDARD_LIBRARY}					\
		appaserver_error.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		google_map_dvr.c					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		appaserver_error.o					\
		environ.o						\
		application.o						\
		appaserver_parameter_file.o				\
		fopen_path.o						\
		application_constants.o					\
		google_map.o						\
	   -o google_map_dvr.e

google_earth_dvr.e: google_earth_dvr.c					\
		google_earth.o 		 				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}
	${CC} ${CFLAGS} -I${LIBRARY}					\
		google_earth_dvr.c					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		google_earth.o						\
	   -o google_earth_dvr.e

histogram_dvr.e: histogram_dvr.c histogram.o
	${CC} ${CFLAGS} -I${LIBRARY} histogram_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		timlib.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		histogram.o 						\
		dictionary.o 						\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		hashtbl.o						\
	   -o histogram_dvr.e

time2day_percentage_dvr.e: time2day_percentage_dvr.c hashtbl.o
	${CC} ${CFLAGS} -I${LIBRARY} time2day_percentage_dvr.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		timlib.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		dictionary.o 						\
		hashtbl.o 						\
	   -o time2day_percentage_dvr.e

julian_dvr.e: julian_dvr.c julian.o hashtbl.o
	${CC} ${CFLAGS} -I${LIBRARY} julian_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		column.o 						\
		date.o 							\
		timlib.o 						\
		piece.o 						\
		list.o 							\
		list_usage.o 						\
		date_convert.o 						\
		dictionary.o 						\
		hashtbl.o 						\
		julian.o						\
	   -o julian_dvr.e

sed_dvr.e: sed_dvr.c sed.o
	${CC} ${CFLAGS} -I${LIBRARY} sed_dvr.c				\
		${APPASERVER_ERROR_FILE_FLAG}				\
		column.o 						\
		date.o 							\
		timlib.o 						\
		piece.o 						\
		list.o 							\
		list_usage.o 						\
		date_convert.o 						\
		dictionary.o 						\
		hashtbl.o 						\
		julian.o						\
		sed.o							\
	   -o sed_dvr.e

linear_interpolate_dvr.e: linear_interpolate_dvr.c timlib.o hashtbl.o
	${CC} ${CFLAGS} linear_interpolate_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		column.o						\
		date.o							\
		julian.o						\
		timlib.o						\
		list.o							\
		list_usage.o						\
		date_convert.o 						\
		dictionary.o						\
		hashtbl.o						\
		piece.o							\
	   -o linear_interpolate_dvr.e

regression_dvr.e: regression_dvr.c					\
		${STANDARD_LIBRARY}
	${CC} ${CFLAGS} -I${LIBRARY} regression_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
	   -o regression_dvr.e

html_invoice_dvr.e: html_invoice_dvr.c					\
	html_invoice.o
	${CC} ${CFLAGS} -I${LIBRARY} html_invoice_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		html_invoice.o						\
	   -o html_invoice_dvr.e

google_chart_dvr.e: google_chart_dvr.c					\
	google_chart.o
	${CC} ${CFLAGS} -I${LIBRARY} google_chart_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
		google_chart.o						\
	   -o google_chart_dvr.e

html_table_dvr.e: html_table_dvr.c
	${CC} ${CFLAGS} -I${LIBRARY} html_table_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
	   -o html_table_dvr.e

date_convert_dvr.e: date_convert_dvr.c date_convert.o
	${CC} ${CFLAGS} -I${LIBRARY} date_convert_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		${APPASERVER_LIBRARY}					\
	   -o date_convert_dvr.e

parse_report_dvr.e: parse_report_dvr.c parse_report.o hashtbl.o
	${CC} ${CFLAGS} -I${LIBRARY} parse_report_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		list.o 							\
		date.o 							\
		julian.o 						\
		document.o 						\
		application.o 						\
		list_usage.o						\
		date_convert.o 						\
		timlib.o						\
		environ.o						\
		dictionary.o						\
		fopen_path.o						\
		decode_html_post.o					\
		column.o						\
		hashtbl.o						\
		piece.o							\
		appaserver_parameter_file.o				\
		parse_report.o						\
	   -o parse_report_dvr.e
	touch ${UTILITY}/parse_report
	rm ${UTILITY}/parse_report
	ln parse_report_dvr.e ${UTILITY}/parse_report

html_table.o: html_table.c html_table.h
	${CC} ${CFLAGS} -c html_table.c					\
		${APPASERVER_ERROR_FILE_FLAG}

html_invoice.o: html_invoice.c html_invoice.h
	${CC} ${CFLAGS} -c html_invoice.c				\
		${APPASERVER_ERROR_FILE_FLAG}

latex.o: latex.c latex.h
	${CC} ${CFLAGS} -c latex.c					\
		${APPASERVER_ERROR_FILE_FLAG}

latex_invoice.o: latex_invoice.c latex_invoice.h
	${CC} ${CFLAGS} -c latex_invoice.c				\
		${APPASERVER_ERROR_FILE_FLAG}

grace.o: grace.c grace.h
	${CC} ${CFLAGS} -I${LIBRARY} -c grace.c				\
		${APPASERVER_ERROR_FILE_FLAG}

easycharts.o: easycharts.c easycharts.h
	${CC} ${CFLAGS} -c easycharts.c					\
		${APPASERVER_ERROR_FILE_FLAG}

pair_one2m.o: pair_one2m.c pair_one2m.h
	${CC} ${CFLAGS} -c pair_one2m.c					\
		${APPASERVER_ERROR_FILE_FLAG}

expected_count.o: expected_count.c expected_count.h julian.o
	${CC} ${CFLAGS} -c expected_count.c				\
		${APPASERVER_ERROR_FILE_FLAG}

julian.o: julian.c julian.h
	${CC} ${CFLAGS} -c julian.c					\
		${APPASERVER_ERROR_FILE_FLAG}

parse_insert_statement.o: parse_insert_statement.c parse_insert_statement.h
	${CC} ${CFLAGS} -c parse_insert_statement.c			\
		${APPASERVER_ERROR_FILE_FLAG}

start_stop_time.o: start_stop_time.c start_stop_time.h
	${CC} ${CFLAGS} -c start_stop_time.c				\
		${APPASERVER_ERROR_FILE_FLAG}

token_dvr.e: token_dvr.c token.o
	${CC} ${CFLAGS} token_dvr.c token.o				\
		${APPASERVER_ERROR_FILE_FLAG}				\
	   -o token_dvr.e

token.o: token.c token.h
	${CC} ${CFLAGS} -c token.c					\
		${APPASERVER_ERROR_FILE_FLAG}

environ_dvr.e: environ_dvr.c environ.o piece.o fopen_path.o hashtbl.o	\
	environ.o
	${CC} ${CFLAGS} -I${LIBRARY} environ_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		environ.o timlib.o 					\
		piece.o list.o fopen_path.o				\
		dictionary.o list_usage.o column.o hashtbl.o		\
		date_convert.o 						\
		date.o julian.o						\
	   -o environ_dvr.e

environ.o: environ.c environ.h timlib.o
	${CC} ${CFLAGS} -I${LIBRARY} -c environ.c			\
		${APPASERVER_ERROR_FILE_FLAG}

maskdvr.e: maskdvr.c mask.o list.o piece.o basename.o
	${CC} ${CFLAGS} maskdvr.c mask.o -o maskdvr.e			\
		${APPASERVER_ERROR_FILE_FLAG}

mask.o: mask.c mask.h
	${CC} ${CFLAGS} -c mask.c					\
		${APPASERVER_ERROR_FILE_FLAG}

reportdvr.e: reportdvr.c report.o hashtbl.o
	${CC} ${CFLAGS} reportdvr.c 		\
		${APPASERVER_ERROR_FILE_FLAG}	\
			piece.o 		\
			date.o 			\
			julian.o 		\
			list.o 			\
			report.o 		\
			using.o 		\
			dictionary.o 		\
			hashtbl.o 		\
			list_usage.o 		\
			date_convert.o 		\
			column.o 		\
			timlib.o 		\
			-o reportdvr.e


name_dvr.e: name_dvr.c name_arg.o list.o piece.o basename.o hashtbl.o
	${CC} ${CFLAGS} name_dvr.c 		\
		${APPASERVER_ERROR_FILE_FLAG}	\
			piece.o 		\
			date.o 			\
			julian.o 		\
			basename.o 		\
			name_arg.o 		\
			dictionary.o 		\
			hashtbl.o 		\
			list_usage.o 		\
			date_convert.o 		\
			column.o 		\
			list.o 			\
			timlib.o 		\
			-o name_dvr.e

name_arg.o: name_arg.c name_arg.h
	${CC} ${CFLAGS} -c name_arg.c		\
		${APPASERVER_ERROR_FILE_FLAG}


date_dvr.e: date_dvr.c					\
		${STANDARD_LIBRARY}			\
		${APPASERVER_LIBRARY}
	${CC} ${CFLAGS} date_dvr.c 			\
			${APPASERVER_ERROR_FILE_FLAG}	\
			${STANDARD_LIBRARY}		\
			${APPASERVER_LIBRARY}		\
			-o date_dvr.e

date.o: date.c date.h
	${CC} ${CFLAGS} -c date.c			\
		${APPASERVER_ERROR_FILE_FLAG}

basename.o: basename.c
	${CC} ${CFLAGS} -c basename.c			\
		${APPASERVER_ERROR_FILE_FLAG}

piece_dvr.e: piece_dvr.c				\
		piece.o					\
		${STANDARD_LIBRARY}			\
		${APPASERVER_LIBRARY}
	${CC} ${CFLAGS} piece_dvr.c			\
		${APPASERVER_ERROR_FILE_FLAG}		\
		${STANDARD_LIBRARY}			\
		${APPASERVER_LIBRARY}			\
	-o piece_dvr.e

piece.o: piece.c piece.h
	${CC} ${CFLAGS} -c piece.c		\
		${APPASERVER_ERROR_FILE_FLAG}

stack.o: stack.c stack.h
	${CC} ${CFLAGS} -c stack.c		\
		${APPASERVER_ERROR_FILE_FLAG}

fifo.o: fifo.c fifo.h
	${CC} ${CFLAGS} -c fifo.c		\
		${APPASERVER_ERROR_FILE_FLAG}

column.o: column.c column.h
	${CC} ${CFLAGS} -c column.c		\
		${APPASERVER_ERROR_FILE_FLAG}

zip_code_dist_dvr.e: zip_code_dist_dvr.c		\
	zip_code_distance.o bsrch.o timlib.o rnd_io.o	\
	piece.o hashtbl.o
	${CC} ${CFLAGS} dist_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}		\
		piece.o 				\
		zip_code_distance.o			\
		list.o 					\
		date.o 					\
		julian.o 				\
		bsrch.o 				\
		list_usage.o 				\
		date_convert.o				\
		dictionary.o 				\
		hashtbl.o 				\
		column.o 				\
		timlib.o 				\
		rnd_io.o 				\
		-o zip_code_dist_dvr.e -lm

distance.o: distance.c distance.h
	${CC} ${CFLAGS} -c distance.c			\
		${APPASERVER_ERROR_FILE_FLAG}

zip_code_distance.o: zip_code_distance.c zip_code_distance.h
	${CC} ${CFLAGS} -c zip_code_distance.c		\
		${APPASERVER_ERROR_FILE_FLAG}

moving_average_dvr.e: moving_average_dvr.c moving_average.o
	${CC} ${CFLAGS} -I${LIBRARY} moving_average_dvr.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		timlib.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		moving_average.o 					\
		dictionary.o 						\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		hashtbl.o						\
	   -o moving_average_dvr.e

expected_count_dvr.e: expected_count_dvr.c expected_count.o
	${CC} ${CFLAGS} -I${LIBRARY} expected_count_dvr.c		\
		${APPASERVER_ERROR_FILE_FLAG}				\
		${STANDARD_LIBRARY}					\
		timlib.o 						\
		date.o 							\
		julian.o 						\
		piece.o 						\
		list.o 							\
		expected_count.o 					\
		dictionary.o 						\
		list_usage.o 						\
		date_convert.o 						\
		column.o 						\
		hashtbl.o						\
	   -o expected_count_dvr.e

moving_average.o: moving_average.c moving_average.h queue.o
	${CC} ${CFLAGS} ${CFLAGS} -c moving_average.c			\
		${APPASERVER_ERROR_FILE_FLAG}

queue_dvr.e: queue_dvr.c queue.o
	${CC} ${CFLAGS} ${CFLAGS} queue_dvr.c queue.o			\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-o queue_dvr.e

queue.o: queue.c queue.h
	${CC} ${CFLAGS} ${CFLAGS} -c queue.c				\
		${APPASERVER_ERROR_FILE_FLAG}

list_dvr.e: list_dvr.c list.o dictionary.o list_usage.o	\
		column.o hashtbl.o decode_html_post.o
	${CC} ${CFLAGS} list_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}		\
			piece.o 			\
			timlib.o 			\
			date.o 				\
			julian.o 			\
			dictionary.o 			\
			list_usage.o 			\
			date_convert.o 			\
			decode_html_post.o 		\
			column.o 			\
			hashtbl.o 			\
			list.o 				\
			-o list_dvr.e

list.o: list.c list.h
	${CC} ${CFLAGS} -c list.c			\
		${APPASERVER_ERROR_FILE_FLAG}

decode_html_post.o: decode_html_post.c decode_html_post.h
	${CC} ${CFLAGS} -c decode_html_post.c		\
		${APPASERVER_ERROR_FILE_FLAG}

list_usage.o: list_usage.c list.h dictionary.o date_convert.o
	${CC} ${CFLAGS} -c list_usage.c			\
		${APPASERVER_ERROR_FILE_FLAG}

dictionary_dvr.e: dictionary_dvr.c dictionary.o hashtbl.o
	${CC} ${CFLAGS} dictionary_dvr.c 		\
		${APPASERVER_ERROR_FILE_FLAG}		\
			dictionary.o 			\
			list_usage.o 			\
			date_convert.o 			\
			column.o 			\
			date.o	 			\
			julian.o 			\
			hashtbl.o 			\
			piece.o 			\
			decode_html_post.o 		\
			timlib.o 			\
			list.o 				\
			-o dictionary_dvr.e

hashdvr.e: hashdvr.c hashtbl.o
	${CC} ${CFLAGS} hashdvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}		\
			piece.o 			\
			date.o 				\
			julian.o 			\
			timlib.o 			\
			hashtbl.o 			\
			dictionary.o 			\
			list_usage.o 			\
			date_convert.o 			\
			column.o 			\
			list.o 				\
			-o hashdvr.e

dictionary.o: dictionary.c dictionary.h hashtbl.o
	${CC} ${CFLAGS} -c dictionary.c			\
		${APPASERVER_ERROR_FILE_FLAG}

hashtbl.o: hashtbl.c hashtbl.h
	${CC} ${CFLAGS} -c hashtbl.c			\
		${APPASERVER_ERROR_FILE_FLAG}

cc_dvr.e: cc_dvr.c cc_chk.o
	${CC} ${CFLAGS} cc_dvr.c cc_chk.o -o cc_dvr.e	\
		${APPASERVER_ERROR_FILE_FLAG}

cc_chk.o: cc_chk.c
	${CC} ${CFLAGS} -c cc_chk.c			\
		${APPASERVER_ERROR_FILE_FLAG}

timlib.o: timlib.c timlib.h
	${CC} -c $(CFLAGS) timlib.c			\
		${APPASERVER_ERROR_FILE_FLAG}

compbear.e: compbear.c
	${CC} ${CFLAGS} -lm compbear.c -o compbear.e	\
		${APPASERVER_ERROR_FILE_FLAG}

std_dvr.e: std_dvr.c std_addr.o 
	${CC} ${CFLAGS} std_dvr.c std_addr.o  -o std_dvr.e	\
		${APPASERVER_ERROR_FILE_FLAG}

std_addr.o: std_addr.c std_addr.h
	${CC} ${CFLAGS} -c std_addr.c				\
		${APPASERVER_ERROR_FILE_FLAG}

using.o: using.c
	${CC} ${CFLAGS} -c using.c				\
		${APPASERVER_ERROR_FILE_FLAG}

bsrchdvr.e: bsrchdvr.c bsrch.o rnd_io.o hashtbl.o
	${CC} ${CFLAGS} bsrchdvr.c 					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		bsrch.o rnd_io.o timlib.o piece.o list.o		\
		list_usage.o dictionary.o hashtbl.o column.o		\
		date_convert.o 						\
		date.o julian.o						\
	-o bsrchdvr.e

bsrch.o: bsrch.c bsrch.h
	${CC} ${CFLAGS} -c bsrch.c					\
		${APPASERVER_ERROR_FILE_FLAG}

date_aggregate.o: date_aggregate.c date_aggregate.h
	${CC} ${CFLAGS} -c date_aggregate.c				\
		${APPASERVER_ERROR_FILE_FLAG}

rnd_io.o: rnd_io.c rnd_io.h
	${CC} ${CFLAGS} -c rnd_io.c					\
		${APPASERVER_ERROR_FILE_FLAG}

julian_time_double_list.o: julian_time_double_list.c julian_time_double_list.h
	${CC} ${CFLAGS} -c julian_time_double_list.c			\
		${APPASERVER_ERROR_FILE_FLAG}

timlib_dvr.e: timlib_dvr.c timlib.o list.o piece.o hashtbl.o	\
		date.o julian.o dictionary.o list_usage.o	\
		column.o
	${CC} ${CFLAGS} timlib_dvr.c 				\
		${APPASERVER_ERROR_FILE_FLAG}			\
			piece.o 				\
			date.o 					\
			julian.o 				\
			list.o 					\
			dictionary.o 				\
			list_usage.o 				\
			column.o 				\
			hashtbl.o 				\
			timlib.o 				\
			date_convert.o 				\
			-o timlib_dvr.e

fopen_path_dvr.e: fopen_path_dvr.c fopen_path.o hashtbl.o	\
	environ.o
	${CC} ${CFLAGS} fopen_path_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}			\
			environ.o 				\
			piece.o 				\
			date.o 					\
			julian.o 				\
			timlib.o 				\
			list.o 					\
			list_usage.o 				\
			date_convert.o 				\
			dictionary.o 				\
			hashtbl.o 				\
			column.o 				\
			fopen_path.o 				\
			-o fopen_path_dvr.e

fopen_path.o: fopen_path.c fopen_path.h
	${CC} ${CFLAGS} -c fopen_path.c				\
		${APPASERVER_ERROR_FILE_FLAG}

update_statement_dvr.e: update_statement_dvr.c 			\
	update_statement.o hashtbl.o
	${CC} ${CFLAGS} update_statement_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}			\
			piece.o 				\
			date.o 					\
			julian.o 				\
			timlib.o 				\
			list_usage.o 				\
			date_convert.o 				\
			dictionary.o 				\
			hashtbl.o 				\
			column.o 				\
			list.o 					\
			update_statement.o 			\
			-o update_statement_dvr.e

update_statement.o: update_statement.c update_statement.h
	${CC} ${CFLAGS} -c update_statement.c			\
		${APPASERVER_ERROR_FILE_FLAG}

appaserver_parameter_file_dvr.e:				\
	appaserver_parameter_file_dvr.c 			\
	appaserver_parameter_file.o hashtbl.o
	${CC} ${CFLAGS} appaserver_parameter_file_dvr.c		\
		${APPASERVER_ERROR_FILE_FLAG}			\
			piece.o 				\
			date.o 					\
			julian.o 				\
			timlib.o 				\
			list_usage.o 				\
			date_convert.o 				\
			dictionary.o 				\
			hashtbl.o 				\
			column.o 				\
			list.o 					\
			appaserver_parameter_file.o 		\
			-o appaserver_parameter_file_dvr

appaserver_parameter_file.o: appaserver_parameter_file.c	\
	appaserver_parameter_file.h
	${CC} ${CFLAGS} -c appaserver_parameter_file.c		\
		${APPASERVER_ERROR_FILE_FLAG}

appaserver_link_file.o: appaserver_link_file.c			\
	appaserver_link_file.h
	${CC} ${CFLAGS} -c appaserver_link_file.c		\
		${APPASERVER_ERROR_FILE_FLAG}

two_way_pipe_dvr.e: two_way_pipe_dvr.c two_way_pipe.o
	${CC} ${CFLAGS} two_way_pipe_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}			\
			piece.o 				\
			date.o 					\
			julian.o 				\
			timlib.o 				\
			list_usage.o 				\
			date_convert.o 				\
			dictionary.o 				\
			hashtbl.o 				\
			column.o 				\
			list.o 					\
			two_way_pipe.o 				\
			-o two_way_pipe_dvr.e

permutation_dvr.e: permutation_dvr.c permutation.o
	${CC} ${CFLAGS} permutation_dvr.c 			\
		${APPASERVER_ERROR_FILE_FLAG}			\
			permutation.o 				\
			-o permutation_dvr.e

permutation.o: permutation.c permutation.h
	${CC} ${CFLAGS} -c permutation.c			\
		${APPASERVER_ERROR_FILE_FLAG}

two_way_pipe.o: two_way_pipe.c two_way_pipe.h
	${CC} ${CFLAGS} -c two_way_pipe.c			\
		${APPASERVER_ERROR_FILE_FLAG}

table.o: table.c table.h
	${CC} ${CFLAGS} -c table.c				\
		${APPASERVER_ERROR_FILE_FLAG}

regression.o: regression.c regression.h
	${CC} ${CFLAGS} -c regression.c				\
		${APPASERVER_ERROR_FILE_FLAG}

document.o: document.c document.h
	${CC} ${CFLAGS} -c document.c				\
		${APPASERVER_ERROR_FILE_FLAG}

application.o: application.c application.h
	${CC} ${CFLAGS} -c application.c			\
		${APPASERVER_ERROR_FILE_FLAG}

appaserver_user.o: appaserver_user.c appaserver_user.h
	${CC} ${CFLAGS} -c appaserver_user.c			\
		${APPASERVER_ERROR_FILE_FLAG}

process_parameter_list.o: process_parameter_list.c			\
		process_parameter_list.h element.h folder.h		\
		populate_helper_process.h
	${CC} ${CFLAGS} -c process_parameter_list.c			\
		${APPASERVER_ERROR_FILE_FLAG}

appaserver.o: appaserver.c appaserver.h					\
		element.o
	${CC} ${CFLAGS} -c appaserver.c					\
		${APPASERVER_ERROR_FILE_FLAG}

folder.o: folder.c folder.h process.h attribute.h date_convert.h	\
		related_folder.o process_generic_output.h		\
		element.o
	${CC} ${CFLAGS} -c folder.c					\
		${APPASERVER_ERROR_FILE_FLAG}

process.o: process.c process.h query.h
	${CC} ${CFLAGS} -c process.c					\
		${APPASERVER_ERROR_FILE_FLAG}

attribute.o: attribute.c attribute.h
	${CC} ${CFLAGS} -c attribute.c					\
		${APPASERVER_ERROR_FILE_FLAG}

element.o: element.c element.h
	${CC} ${CFLAGS} -c element.c					\
		${APPASERVER_ERROR_FILE_FLAG}

pipe.o: pipe.c pipe.h
	${CC} ${CFLAGS} -c pipe.c					\
		${APPASERVER_ERROR_FILE_FLAG}

operation.o: operation.c operation.h process.h
	${CC} ${CFLAGS} -c operation.c					\
		${APPASERVER_ERROR_FILE_FLAG}

form.o: form.c form.h element.h dictionary_appaserver.h
	${CC} ${CFLAGS} -c form.c					\
		${APPASERVER_ERROR_FILE_FLAG}

query_dvr.e: query_dvr.c query.o
	${CC} ${CFLAGS} query_dvr.c				\
			${APPASERVER_ERROR_FILE_FLAG}		\
			${APPASERVER_LIBRARY}			\
			piece.o 				\
			date.o 					\
			julian.o 				\
			timlib.o 				\
			list_usage.o 				\
			date_convert.o 				\
			dictionary.o 				\
			hashtbl.o 				\
			column.o 				\
			list.o 					\
			-o query_dvr.e

query.o: query.c query.h related_folder.h
	${CC} ${CFLAGS} -c query.c					\
		${APPASERVER_ERROR_FILE_FLAG}

prompt_recursive.o: prompt_recursive.c					\
	prompt_recursive.h related_folder.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c prompt_recursive.c

related_folder.o: related_folder.c related_folder.h folder.h		\
			attribute.h form.h
	${CC} ${CFLAGS} -c related_folder.c				\
		${APPASERVER_ERROR_FILE_FLAG}

role_folder.o: role_folder.c role_folder.h folder.h
	${CC} ${CFLAGS} -c role_folder.c				\
		${APPASERVER_ERROR_FILE_FLAG}

validation_form.o: validation_form.c validation_form.h element.h
	${CC} ${CFLAGS} -c validation_form.c				\
		${APPASERVER_ERROR_FILE_FLAG}

post2dictionary.o: post2dictionary.c post2dictionary.h
	${CC} ${CFLAGS} -c post2dictionary.c				\
		${APPASERVER_ERROR_FILE_FLAG}

period_wo_date.o: period_wo_date.c period_wo_date.h timlib.h
	${CC} ${CFLAGS} -c period_wo_date.c				\
		${APPASERVER_ERROR_FILE_FLAG}

dictionary2file.o: dictionary2file.c dictionary2file.h
	${CC} ${CFLAGS} -c dictionary2file.c				\
		${APPASERVER_ERROR_FILE_FLAG}

application_constants.o: application_constants.c application_constants.h
	${CC} ${CFLAGS} -c application_constants.c			\
		${APPASERVER_ERROR_FILE_FLAG}

session.o: session.c session.h
	${CC} ${CFLAGS} -c session.c					\
		${APPASERVER_ERROR_FILE_FLAG}

waypoint.o: waypoint.c waypoint.h
	${CC} ${CFLAGS} -c waypoint.c					\
		${APPASERVER_ERROR_FILE_FLAG}

pivot_table.o: pivot_table.c pivot_table.h
	${CC} ${CFLAGS} -c pivot_table.c				\
		${APPASERVER_ERROR_FILE_FLAG}

generic_load.o: generic_load.c generic_load.h
	${CC} ${CFLAGS} -c generic_load.c				\
		${APPASERVER_ERROR_FILE_FLAG}

report_writer.o: report_writer.c report_writer.h
	${CC} ${CFLAGS} -c report_writer.c				\
		${APPASERVER_ERROR_FILE_FLAG}

scatterplot.o: scatterplot.c scatterplot.h
	${CC} ${CFLAGS} -c scatterplot.c				\
		${APPASERVER_ERROR_FILE_FLAG}

role.o: role.c role.h
	${CC} ${CFLAGS} -c role.c					\
		${APPASERVER_ERROR_FILE_FLAG}

sed.o: sed.c sed.h
	${CC} ${CFLAGS} -c sed.c					\
		${APPASERVER_ERROR_FILE_FLAG}

operation_list.o: operation_list.c operation_list.h operation.h
	${CC} ${CFLAGS} -c operation_list.c				\
		${APPASERVER_ERROR_FILE_FLAG}

frameset.o: frameset.c frameset.h
	${CC} ${CFLAGS} -c frameset.c					\
		${APPASERVER_ERROR_FILE_FLAG}

insert_database.o: insert_database.c insert_database.h			\
	process.h
	${CC} ${CFLAGS} -c insert_database.c				\
		${APPASERVER_ERROR_FILE_FLAG}

update_database.o: update_database.c update_database.h
	${CC} ${CFLAGS} -c update_database.c				\
		${APPASERVER_ERROR_FILE_FLAG}

delete_database.o: delete_database.c delete_database.h
	${CC} ${CFLAGS} -c delete_database.c				\
		${APPASERVER_ERROR_FILE_FLAG}

query_attribute_statistics_list.o:					\
		query_attribute_statistics_list.c			\
		query_attribute_statistics_list.h
	${CC} ${CFLAGS} -I${LIBRARY} 					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c query_attribute_statistics_list.c

aggregate_level.o: aggregate_level.c aggregate_level.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c aggregate_level.c

validation_level.o: validation_level.c validation_level.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c validation_level.c

appaserver_library.o: appaserver_library.c appaserver_library.h		\
	element.h related_folder.h application.h date_convert.h		\
	process_generic_output.h appaserver.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c appaserver_library.c

appaserver_error.o: appaserver_error.c appaserver_error.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c appaserver_error.c

aggregate_statistic.o: aggregate_statistic.c aggregate_statistic.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c aggregate_statistic.c

make_date_time_between_compatable.o: make_date_time_between_compatable.c \
	make_date_time_between_compatable.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c make_date_time_between_compatable.c

populate_helper_process.o: populate_helper_process.c 			\
	populate_helper_process.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c populate_helper_process.c

lookup_before_drop_down.o: lookup_before_drop_down.c 			\
	lookup_before_drop_down.h					\
	form.h								\
	related_folder.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c lookup_before_drop_down.c

process_generic_output.o: process_generic_output.c 			\
	process_generic_output.h query.h				\
	attribute.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c process_generic_output.c

horizontal_menu.o: horizontal_menu.c					\
	horizontal_menu.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c horizontal_menu.c

folder_menu.o: folder_menu.c						\
	folder_menu.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c folder_menu.c

google_map.o: google_map.c google_map.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c google_map.c

google_earth.o: google_earth.c google_earth.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c google_earth.c

google_chart.o: google_chart.c google_chart.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c google_chart.c

covariance.o: covariance.c covariance.h list.h hashtbl.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c covariance.c

semaphore.o: semaphore.c semaphore.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c semaphore.c

dollar.o: dollar.c dollar.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c dollar.c

fork_control.o: fork_control.c fork_control.h list.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c fork_control.c

histogram.o: histogram.c histogram.h list.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c histogram.c

parse_report.o: parse_report.c parse_report.h list.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c parse_report.c

statistics_weighted.o: statistics_weighted.c statistics_weighted.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c statistics_weighted.c

additional_drop_down_attribute.o:					\
		additional_drop_down_attribute.c			\
		additional_drop_down_attribute.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c additional_drop_down_attribute.c

date_convert.o: date_convert.c date_convert.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c date_convert.c

row_security.o: row_security.c row_security.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c row_security.c

dictionary_appaserver.o: dictionary_appaserver.c			\
	dictionary_appaserver.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c dictionary_appaserver.c

post_login_library.o: post_login_library.c				\
	post_login_library.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c post_login_library.c

array.o: array.c							\
	array.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c array.c

mysqldump.o: mysqldump.c						\
	mysqldump.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c mysqldump.c

keys_match_sum.o: keys_match_sum.c keys_match_sum.h			\
	mysqldump.h
	${CC} ${CFLAGS} -I${LIBRARY}					\
		${APPASERVER_ERROR_FILE_FLAG}				\
		-c keys_match_sum.c

ctemplate_wrapper.o: ctemplate_wrapper.cpp					\
		${EXTERNAL_LIBS} -lctemplate_nothreads 
