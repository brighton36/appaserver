#!/bin/sh
#create equity_account_balance

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != timriley -a		\
     "$application" != eves -a			\
     "$application" != capitolpops -a		\
     "$application" != appahost ]
then
	exit 0
fi

table_name=`get_table_name $application equity_account_balance`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,date_time datetime not null,investment_operation char (15),transaction_date_time datetime,share_price double (12,4),share_quantity_change double (12,4),share_quantity_balance double (12,4),book_value_change double (12,4),book_value_balance double (13,4),moving_share_price double (13,5),total_cost_balance double (13,5),market_value double (13,4),unrealized_gain_balance double (12,4),unrealized_gain_change double (12,4),realized_gain double (12,4)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,account_number,date_time);" | sql.e '^' mysql

table_name=`get_table_name $application financial_institution`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql

table_name=`get_table_name $application investment_account`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,classification char (15),duration_term char (15),certificate_maturity_months integer,certificate_maturity_date date,interest_rate double (5,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,account_number);" | sql.e '^' mysql

table_name=`get_table_name $application investment_classification`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (classification char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (classification);" | sql.e '^' mysql

table_name=`get_table_name $application investment_operation`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (investment_operation char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (investment_operation);" | sql.e '^' mysql

table_name=`get_table_name $application duration_term`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (duration_term char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (duration_term);" | sql.e '^' mysql


