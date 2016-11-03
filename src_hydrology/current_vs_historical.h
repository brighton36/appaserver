/* current_vs_historical.h */
/* ----------------------- */

#ifndef CURRENT_VS_HISTORICAL_H
#define CURRENT_VS_HISTORICAL_H

#include <stdio.h>

/* Constants */
/* --------- */
#define HISTORICAL_RANGE_YEARS			5
#define EXCLUDE_DATATYPE_NAME_LIST_STRING	"voltage,power,cr10,null,salinity_paleo"
#define MESSAGE_LEFT_POSITION			265
#define MAP_POSITION_TOP			25
#define MAP_POSITION_LEFT			220
#define CHART_POSITION_TOP			50
#define CHART_POSITION_LEFT			265
#define CHART_WIDTH				800
#define HISTORICAL_CHART_HEIGHT			400
#define CURRENT_CHART_HEIGHT			500
#define BACKGROUND_COLOR			"#effdff"
#define MONTH_LIST_STRING "jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec"
#define DATATYPE_QUANTUM_TOTAL			"total"
#define LONG_TERM_CURRENT			"current"
#define LONG_TERM_HISTORICAL			"historical"
#define DATATYPE_AGGREGATION_DELIMITER		'/'
#define AGGREGATION_SUM				"sum"
#define AGGREGATION_AVG				"avg"
#define AGGREGATION_MIN				"min"
#define AGGREGATION_MAX				"max"
#define DATATYPE_RADIO_BUTTON_IDENTIFIER	"current_vs_historical_radio"
#define CURRENT_VS_HISTORICAL_STATION_PREFIX	"station_list_element"
/*
#define OUTPUT_FILE_TEMPLATE	"%s/%s/current_vs_historical_%s.html"
#define HTTP_FILE_TEMPLATE	"%s://%s/%s/current_vs_historical_%s.html"
*/
#define ADDITIONAL_ONLOAD_CONTROL_STRING	"DynarchMenu.setup( 'menu', {electric: 250, blink: false,lazy: true, scrolling: true})"

/* Enum types */
/* ---------- */
enum state {			initial,
				post_map,
				map,
				current,
				historical,
				unknown };

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
enum state current_vs_historical_get_state(
				char *state_string );

char *current_vs_historical_get_state_string(
				enum state );

void current_vs_historical_get_dates(
			char **por_historical_begin_date_string,
			char **por_historical_end_date_string,
			char **por_current_begin_date_string,
			char **por_current_end_date_string,
			char **current_begin_date_string,
			char **current_end_date_string,
			char *application_name );

#endif
