#!/bin/sh

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != timriley ]
then
	exit 0
fi

#create financial_institution
table_name=`get_table_name $application financial_institution`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address);" | sql.e '^' mysql

#create investment_account
table_name=`get_table_name $application investment_account`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,classification char (15),duration_term char (15),certificate_maturity_months integer,certificate_maturity_date date,interest_rate double (5,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,account_number);" | sql.e '^' mysql

#create investment_classification
table_name=`get_table_name $application investment_classification`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (classification char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (classification);" | sql.e '^' mysql

#create duration_term
table_name=`get_table_name $application duration_term`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (duration_term char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (duration_term);" | sql.e '^' mysql

#create account_balance
table_name=`get_table_name $application account_balance`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,date date not null,investment_operation char (15),balance double (11,2),balance_change double (11,2),transaction_date_time datetime) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,account_number,date);" | sql.e '^' mysql

#create investment_operation
table_name=`get_table_name $application investment_operation`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (investment_operation char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (investment_operation);" | sql.e '^' mysql

