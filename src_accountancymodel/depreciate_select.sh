:
select="full_name,street_address,purchase_date_time,asset_name,serial_number,estimated_useful_life_years,estimated_useful_life_units,estimated_residual_value,declining_balance_n,depreciation_method,accumualated_depreciation"

where="ifnull(extension,0) <> ifnull(accumulated_depreciation,0) and disposal_date is null"

folder="fixed_asset_purchase"

echo "select $select from $folder where $where;"		|
sql.e								|
sort

exit 0
