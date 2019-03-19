#!/bin/sh

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
   echo "Error in `basename.e $0 n`: you must first:" 1>&2
   echo "$ . set_database" 1>&2
   exit 1
fi

if [ "$application" != "hydrology" -a "$application" != "audubon" ]
then
	exit 0
fi

table_name=datatype_alias
(
sql.e << all_done
insert into $table_name (datatype_alias,datatype) values ('Specific Conductance','sp_conductance');
insert into $table_name (datatype_alias,datatype) values ('Temperature','temperature_water');
insert into $table_name (datatype_alias,datatype) values ('Water Level','stage');
insert into $table_name (datatype_alias,datatype) values ('TA','air_temperature');
insert into $table_name (datatype_alias,datatype) values ('BSdT','Bottom_SondeTemp');
insert into $table_name (datatype_alias,datatype) values ('TW','bottom_temperature');
insert into $table_name (datatype_alias,datatype) values ('TWPZ','bottom_temperature');
insert into $table_name (datatype_alias,datatype) values ('BTM','bottom_temperature_max');
insert into $table_name (datatype_alias,datatype) values ('BTN','bottom_temperature_min');
insert into $table_name (datatype_alias,datatype) values ('BTV','bottom_temp_hrly_avg');
insert into $table_name (datatype_alias,datatype) values ('HM','campbell_collection_time');
insert into $table_name (datatype_alias,datatype) values ('WC','cond2');
insert into $table_name (datatype_alias,datatype) values ('CSPZ','conductivity');
insert into $table_name (datatype_alias,datatype) values ('YF','forward_power');
insert into $table_name (datatype_alias,datatype) values ('OUT','radar_outliers');
insert into $table_name (datatype_alias,datatype) values ('STD','radar_standard_deviation');
insert into $table_name (datatype_alias,datatype) values ('PP','rain');
insert into $table_name (datatype_alias,datatype) values ('YR','reflected_power');
insert into $table_name (datatype_alias,datatype) values ('HP','stage');
insert into $table_name (datatype_alias,datatype) values ('HH','stage2');
insert into $table_name (datatype_alias,datatype) values ('ST','surface_temperature');
insert into $table_name (datatype_alias,datatype) values ('TWP0','surface_temperature');
insert into $table_name (datatype_alias,datatype) values ('TTM','surface_temperature_max');
insert into $table_name (datatype_alias,datatype) values ('TTN','surface_temperature_min');
insert into $table_name (datatype_alias,datatype) values ('TTV','surface_temp_hrly_avg');
insert into $table_name (datatype_alias,datatype) values ('PT','temperature_water');
insert into $table_name (datatype_alias,datatype) values ('BV','voltage');
insert into $table_name (datatype_alias,datatype) values ('BL','voltage_min');
insert into $table_name (datatype_alias,datatype) values ('YB','voltage_min');
all_done
) 2>&1 | grep -vi duplicate
exit 0
