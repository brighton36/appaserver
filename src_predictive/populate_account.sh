#!/bin/bash
# ---------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/populate_account.sh
# ---------------------------------------------------------
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application one2m_folder subclassification" 1>&2
	exit 1
fi

echo $0 $* 1>&2

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

one2m_folder=$2
subclassification=$3

table=$(get_table_name $application account)
select="account"

if [ "$subclassification" = "" -o "$subclassification" = "subclassification" ]
then
	where="1 = 1"
else
	where="subclassification = '$subclassification'"
fi

if [ "$one2m_folder" = "inventory" ]
then
	where="subclassification = 'inventory' or subclassification = 'cost_of_goods_sold'"
fi

if [ "$one2m_folder" = "investment_account" ]
then
	where="subclassification like '%investment%'"
fi

if [ "$one2m_folder" = "fixed_asset" ]
then
	where="subclassification = 'property_plant_equipment'"
fi

if [	"$one2m_folder" = "fixed_service" -o	\
	"$one2m_folder" = "hourly_service" 	]
then
	where="subclassification = 'revenue'"
fi

if [ "$one2m_folder" = "liability_account_entity" ]
then
	where="subclassification = 'current_liability'"
fi

if [ "$one2m_folder" = "supply" ]
then
	where="subclassification = 'supply_expense'"
fi

if [ "$one2m_folder" = "service_purchase" ]
then
	where="subclassification = 'service_expense'"
fi

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
