#!/bin/bash
# ---------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_account.sh
# ---------------------------------------------------------

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
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 one2m_folder subclassification [element]" 1>&2
	exit 1
fi

echo $0 $* 1>&2

one2m_folder=$1
subclassification="$2"

if [ "$#" -eq 3 -a "$3" != "" -a "$3" != "element" ]
then
	element="$3"
else
	element=""
fi

select="concat( account, '|', account, '---', ifnull( chart_account_number, '' ) )"
#select="concat( account, '|', account )"
from="account"
order="account"

if [ "$subclassification" = "" -o "$subclassification" = "subclassification" ]
then
	subclassification_where="1 = 1"
else
	subclassification_where="subclassification = '$subclassification'"
fi

if [ "$element" != "" ]
then
	subclassification_join="account.subclassification = subclassification.subclassification"

	element_portion="subclassification.element = 'expense'"

	element_where="${subclassification_join} and ${element_portion}"

	# Override from clause
	# --------------------
	from="account,subclassification"
else
	element_where="1 = 1"
fi

one2m_where="1 = 1"

if [ "$one2m_folder" = "inventory" ]
then
	one2m_where="account.subclassification = 'inventory' or account.subclassification = 'cost_of_goods_sold'"
fi

if [ "$one2m_folder" = "investment_account" ]
then
	one2m_where="account.subclassification like '%investment%'"
fi

if [ "$one2m_folder" = "fixed_asset" ]
then
	one2m_where="account.subclassification = 'property_plant_equipment' or account.subclassification = 'fixed_asset'"
fi

if [	"$one2m_folder" = "fixed_service" -o	\
	"$one2m_folder" = "hourly_service" 	]
then
	one2m_where="account.subclassification = 'revenue'"
fi

if [ "$one2m_folder" = "liability_account_entity" ]
then
	one2m_where="account.subclassification = 'current_liability'"
fi

if [ "$one2m_folder" = "supply" ]
then
	one2m_where="account.subclassification = 'supply_expense'"
fi

if [ "$one2m_folder" = "service_purchase" ]
then
	one2m_where="account.subclassification = 'service_expense'"
fi

where="$one2m_where and $subclassification_where and $element_where"

echo "select $select from $from where $where order by $order;"		|
sql.e									|
cat

exit 0
