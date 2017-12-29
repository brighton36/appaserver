system( "now_nano.sh | cut -b 1-13 >> /var/log/appaserver/appaserver_eves.err" );
{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d: finished = %s\n",
__FILE__,
__FUNCTION__,
__LINE__,
"entity_historical_customer_sale_list_transaction_update()" );
m2( application_name, msg );
}

