{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d: got where_clause = (%s)",
__FILE__,
__FUNCTION__,
__LINE__,
where_clause );
m( msg );
}
