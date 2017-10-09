{
char msg[ 65536 ];
sprintf( msg, "\n%s/%s()/%d: got where_clause = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
where_clause );
m2( "hydrology", msg );
}
