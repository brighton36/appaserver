:
select="full_name,street_address,purchase_date_time,asset_name,serial_number"
where="ifnull(extension,0) <> ifnull(accumulated_depreciation,0)"
folder="fixed_asset_purchase"

echo "select $select from $folder where $where;"		|
sql.e

exit 0
