:
select="full_name,street_address"
where="ifnull(extension,0) <> ifnull(accumulated_depreciation,0)"
folder="fixed_asset_purchase"

echo "select $select from $folder where $where;"		|
sql.e								|
sort -u

exit 0
