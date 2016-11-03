#ifdef NOT_DEFINED
void post_change_depreciation_asset_serial_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			char *depreciation_date,
			char *preupdate_asset_name,
			char *preupdate_serial_number )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_FIXED_ASSET *purchase_fixed_asset;
	DEPRECIATION *depreciation;

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

/*
	depreciation =
		depreciation_fetch(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			depreciation_date );

	if ( !depreciation )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot find depreciation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
*/

	purchase_fixed_asset =
		purchase_fixed_asset_fetch(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number );

	if ( !purchase_fixed_asset )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot find purchase_fixed_asset.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !depreciation->transaction )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	purchase_fixed_asset_depreciation_propagate(
		purchase_fixed_asset,
		puarrived_date_time,
				char *application_name );

} /* post_change_depreciation_asset_serial_update() */
#endif

