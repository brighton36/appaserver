#!/bin/sh
# ---------------------------------
# width4attribute.sh
# ---------------------------------
#
# Copyright 2000-01 Tim Riley
# ---------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo \
"Usage: $0 entity attribute " 1>&2
	exit 1
fi

entity=$1
attribute=$2

attribute_table=`get_table_name $entity attribute`

echo "select width							\
      from $attribute_table						\
      where $attribute_table.attribute = '$attribute';"			|
sql.e

