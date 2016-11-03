void add_valid_value( NAME_ARG *arg,int ticket, char *value)
{
        struct valid_option *find_ticket(), *v_option = find_ticket(arg,ticket);


        if (v_option)
        {
                if ( !add_item( v_option->valid_value,
                                value,
                                strlen(value) + 1,
                                ADD_TAIL) )
                {
                        fprintf( stderr, "malloc failed in add_valid_value\n" );

                        exit( 1 );
                }
        }
        else
                bye ("Can't Find Ticket Number");
} /* end add_valid_value */

