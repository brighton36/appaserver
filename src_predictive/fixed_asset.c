/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.c			*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks fixed_asset ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

FIXED_ASSET *fixed_asset_new( void )
{
	FIXED_ASSET *p =
		(FIXED_ASSET *)
			calloc( 1, sizeof( FIXED_ASSET ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;

} /* fixed_asset_new() */

char *fixed_asset_get_select( char *folder_name )
{
	char select[ 512 ];

	sprintf( select,
"%s.asset_name,%s.serial_number,account,service_placement_date,extension,estimated_useful_life_years,estimated_useful_life_units,estimated_residual_value,declining_balance_n,depreciation_method,tax_cost_basis,tax_recovery_period,disposal_date,finance_accumulated_depreciation,tax_accumulated_depreciation",
		 folder_name,
		 folder_name );

	return strdup( select) ;
}

FIXED_ASSET *fixed_asset_parse( char *input_buffer )
{
	FIXED_ASSET *fixed_asset;
	char piece_buffer[ 256 ];

	fixed_asset = fixed_asset_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	fixed_asset->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	fixed_asset->serial_number = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *piece_buffer )
		fixed_asset->account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *piece_buffer )
		fixed_asset->tax_service_placement_date =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		fixed_asset->extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		fixed_asset->estimated_useful_life_years =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		fixed_asset->estimated_useful_life_units =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		fixed_asset->estimated_residual_value =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		fixed_asset->declining_balance_n =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		fixed_asset->depreciation_method =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	if ( *piece_buffer )
		fixed_asset->tax_cost_basis =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	if ( *piece_buffer )
		fixed_asset->tax_recovery_period =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	if ( *piece_buffer )
		fixed_asset->disposal_date =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 13 );
	if ( *piece_buffer )
		fixed_asset->finance_accumulated_depreciation =
		fixed_asset->
			database_finance_accumulated_depreciation =
				atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 14 );
	if ( *piece_buffer )
		fixed_asset->tax_accumulated_depreciation =
		fixed_asset->database_tax_accumulated_depreciation =
			atof( piece_buffer );

	return fixed_asset;

} /* fixed_asset_parse() */

FIXED_ASSET *fixed_asset_purchase_fetch(
					char *application_name,
					char *asset_name,
					char *serial_number )
{
	char sys_string[ 2048 ];
	char buffer[ 256 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder;
	char *results;
	FIXED_ASSET *fixed_asset;

	select =
		fixed_asset_get_select(
			"fixed_asset_purchase"
				/* folder_name */ );

	folder = "fixed_asset_purchase,fixed_asset";

	join_where = "fixed_asset_purchase.asset_name = fixed_asset.asset_name";

	sprintf( where,
"fixed_asset_purchase.asset_name = '%s' and serial_number = '%s' and %s",
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number,
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (FIXED_ASSET *)0;
	}

	fixed_asset = fixed_asset_parse( results );

	fixed_asset->depreciation_list =
		depreciation_fetch_list(
			application_name,
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* purchase_date_time */,
			fixed_asset->asset_name,
			fixed_asset->serial_number );

	purchase_fixed_asset->tax_recovery_list =
		tax_recovery_fetch_list(
			application_name,
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* purchase_date_time */,
			fixed_asset->asset_name,
			fixed_asset->serial_number );

	return fixed_asset;

} /* fixed_asset_purchase_fetch() */

LIST *fixed_asset_purchase_fetch_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *ledger_where;
	char *join_where;
	char *select;
	char *folder;
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	FIXED_ASSET *fixed_asset;
	LIST *fixed_asset_purchase_list;

	select =
		fixed_asset_get_select(
			"fixed_asset_purchase"
				/* folder_name */ );

	folder = "fixed_asset_purchase,fixed_asset";

	join_where = "fixed_asset_purchase.asset_name = fixed_asset.asset_name";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where, "%s and %s", ledger_where, join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	fixed_asset_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		fixed_asset =
			fixed_asset_parse(
				input_buffer );

		fixed_asset->depreciation_list =
			depreciation_fetch_list(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				fixed_asset->asset_name,
				fixed_asset->serial_number );

		purchase_fixed_asset->tax_recovery_list =
			tax_recovery_fetch_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* purchase_date_time */,
				fixed_asset->asset_name,
				fixed_asset->serial_number );

		list_append_pointer(	fixed_asset_purchase_list,
					fixed_asset );
	}

	pclose( input_pipe );
	return fixed_asset_purchase_list;

} /* fixed_asset_purchase_fetch_list() */

