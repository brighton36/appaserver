# src_rentalproperty/makefile
# -----------------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library
ACCOUNTANCYMODEL_LIBRARY=${APPASERVER_HOME}/src_accountancymodel

STANDARD_LIBRARY_OBJECT=						\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=	schedule_e_report					\
		post_rental_property_reoccurring_transaction

EXTERNAL_LIBRARY_OBJECT=						\
		${STANDARD_LIBRARY_OBJECT}				\
		${LIBRARY}/date_aggregate.o 				\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o 			\
		${LIBRARY}/html_table.o 				\
		${LIBRARY}/prompt_recursive.o 				\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/query.o					\
		${LIBRARY}/latex.o					\
		${LIBRARY}/process.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/element.o					\
		${LIBRARY}/operation.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/session.o					\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o

SOURCE_LIBRARY_OBJECT= 							\
		${ACCOUNTANCYMODEL_LIBRARY}/ledger.o			\
		${ACCOUNTANCYMODEL_LIBRARY}/purchase.o			\
		${ACCOUNTANCYMODEL_LIBRARY}/customer.o			\
		${ACCOUNTANCYMODEL_LIBRARY}/inventory.o			\
		${ACCOUNTANCYMODEL_LIBRARY}/depreciation.o		\
		${ACCOUNTANCYMODEL_LIBRARY}/entity.o

all: 	${SOURCE_LIBRARY_OBJECT}					\
	${EXTENRAL_LIBRARY_OBJECT}					\
	${EXECUTABLES}

clean:
	rm ${EXECUTABLES} *.o

post_rental_property_reoccurring_transaction:				\
		post_rental_property_reoccurring_transaction.c		\
	${EXTERNAL_LIBRARY_OBJECT} ${SOURCE_LIBRARY_OBJECT} rental.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${ACCOUNTANCYMODEL_LIBRARY}				\
		${EXTERNAL_LIBRARY_OBJECT}				\
		${SOURCE_LIBRARY_OBJECT}				\
		rental.o						\
		post_rental_property_reoccurring_transaction.c		\
		-o post_rental_property_reoccurring_transaction

schedule_e_report: schedule_e_report.c					\
	${EXTERNAL_LIBRARY_OBJECT} ${SOURCE_LIBRARY_OBJECT} rental.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${ACCOUNTANCYMODEL_LIBRARY}				\
		${EXTERNAL_LIBRARY_OBJECT}				\
		${SOURCE_LIBRARY_OBJECT}				\
		rental.o						\
		schedule_e_report.c					\
		-o schedule_e_report

rental.o: rental.c rental.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-I${ACCOUNTANCYMODEL_LIBRARY}				\
		-c rental.c

