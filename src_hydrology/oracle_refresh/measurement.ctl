options (silent=feedback, rows=500, bindsize=999999, errors=999999999)
load data
infile measurement.fifo
insert
into table measurement
fields terminated by '|'
(
	station,
	datatype,
	measurement_date date "DD-MON-YYYY",
	measurement_time,
	measurement_date_time date "DD-MON-YYYY:HH24MI",
	measurement_value,
	last_validation_date,
	last_person_validating,
	measurement_update_method,
	last_validation_process,
	reason_value_missing,
	delta_time
)
