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

# First do ysi_load_heading
# -------------------------
select="datatype,ysi_load_heading"
from="datatype"
where="ysi_load_heading is not null"

table=datatype_alias
field="datatype,datatype_alias"

echo "select $select from $from where $where;"		|
sql.e							|
insert_statement.e table=$table field=$field del='^'	|
sql.e 2>&1 | grep -vi duplicate				|
cat

# Second do exo_load_heading
# --------------------------
select="datatype,exo_load_heading"
from="datatype"
where="exo_load_heading is not null"

table=datatype_alias
field="datatype,datatype_alias"

echo "select $select from $from where $where;"		|
sql.e							|
insert_statement.e table=$table field=$field del='^'	|
sql.e 2>&1 | grep -vi duplicate				|
cat

exit 0
