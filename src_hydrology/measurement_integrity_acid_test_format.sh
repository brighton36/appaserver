#!/bin/bash
# src_hydrology/measurement_integrity_acid_test_format.sh
# -------------------------------------------------------
# Freely available software: see Appaserver.org
# -------------------------------------------------------

sed 's/min=//'							|
sed 's/avg=//'							|
sed 's/max=//'							|
delimiter2padded_columns.e '^'

exit 0

