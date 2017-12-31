#!/bin/sh

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi
application=$1

if [ "$application" != appahost -a "$application" != eves ]
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
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,classification char (15),investment_account char (60),fair_value_adjustment_account char(60),certificate_maturity_months integer,certificate_maturity_date date,interest_rate double (5,2)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,account_number);" | sql.e '^' mysql

#create investment_classification
table_name=`get_table_name $application investment_classification`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (classification char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (classification);" | sql.e '^' mysql

#create investment_operation
table_name=`get_table_name $application investment_operation`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (investment_operation char (15) not null) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (investment_operation);" | sql.e '^' mysql

table_name=`get_table_name $application equity_account_balance`
echo "drop table if exists $table_name;" | sql.e '^'
echo "create table $table_name (full_name char (60) not null,street_address char (40) not null,account_number char (50) not null,date_time datetime not null,investment_operation char (15),transaction_date_time datetime,share_price double (12,4),share_quantity_change double (12,4),share_quantity_balance double (13,4),book_value_change double (12,4),book_value_balance double (13,4),moving_share_price double (13,5),total_cost_balance double (14,5),market_value double (13,4),unrealized_gain_change double (12,4),unrealized_gain_balance double (12,4),realized_gain double (12,4)) engine MyISAM;" | sql.e '^' mysql
echo "create unique index $table_name on $table_name (full_name,street_address,account_number,date_time);" | sql.e '^' mysql

table_name=`get_table_name $application investment_operation`
(
sql.e << all_done
delete from $table_name;
insert into $table_name (investment_operation) values ('purchase');
insert into $table_name (investment_operation) values ('sale');
insert into $table_name (investment_operation) values ('time_passage');
all_done
) 2>&1 | grep -vi duplicate

table_name=`get_table_name $application investment_classification`
(
sql.e << all_done
delete from $table_name;
insert into $table_name (classification) values ('certificate');
insert into $table_name (classification) values ('checking');
insert into $table_name (classification) values ('credit_card');
insert into $table_name (classification) values ('money_market');
insert into $table_name (classification) values ('mutual_fund');
insert into $table_name (classification) values ('savings');
insert into $table_name (classification) values ('stock');
all_done
) 2>&1 | grep -vi duplicate
