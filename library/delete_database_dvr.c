/* library/delete_database_dvr.c */
/* ----------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "delete_database.h"

void test_benthic_project( void );
void test_benthic_appaserver_user( void );
void test_shef( void );
void test_capitolpops_band_member( boolean dont_delete_mto1_isa );
void test_capitolpops_section( void );
void test_capitolpops_musician( char *full_name );

int main()
{
	test_benthic_project();
	/* test_benthic_appaserver_user(); */
	/* test_shef(); */
	/* test_capitolpops_musician( "Jay Paulus" );*/
	/* test_capitolpops_band_member( 0 ); */
	/* test_capitolpops_band_member( 1 ); */
	/* test_capitolpops_section(); */

	return 0;
} /* main() */

void test_benthic_project( void )
{
	DELETE_DATABASE *delete_database;
	char *application_name = "benthic";
	char *root_folder_name = "project";
	char *primary_attribute_data_list_string = "FIAN Update";
	char delimiter = DELETE_DATABASE_DATA_DELIMITER;

	delete_database = 
		delete_database_new(
			application_name,
			0 /* not dont_delete_mto1_isa */,
			root_folder_name,
			primary_attribute_data_list_string,
			delimiter );

	if ( ! ( delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			delete_database->delete_folder_list,
			application_name,
			delete_database->dont_delete_mto1_isa,
			delete_database->root_folder_name,
			delete_database->root_primary_attribute_data_list ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: for folder = (%s), have empty delete_folder_list for root_primary_attribute_data_list = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delete_database->root_folder_name,
			 list_display(
				delete_database->
					root_primary_attribute_data_list ) );
		exit( 1 );
	}

/*
printf( "%s\n", delete_database_display( delete_database ) );
*/

	delete_database_execute_delete_folder_list(
			delete_database->application_name,
			delete_database->delete_folder_list,
			"cat" /* sql_executable */ );

} /* test_benthic_project() */

void test_benthic_appaserver_user( void )
{
	DELETE_DATABASE *delete_database;
	char *application_name = "benthic";
	char *root_folder_name = "appaserver_user";
	char *primary_attribute_data_list_string = "robin";
	char delimiter = DELETE_DATABASE_DATA_DELIMITER;

	delete_database = 
		delete_database_new(
			application_name,
			0 /* not dont_delete_mto1_isa */,
			root_folder_name,
			primary_attribute_data_list_string,
			delimiter );

	if ( ! ( delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			delete_database->delete_folder_list,
			application_name,
			delete_database->dont_delete_mto1_isa,
			delete_database->root_folder_name,
			delete_database->root_primary_attribute_data_list ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: for folder = (%s), have empty delete_folder_list for root_primary_attribute_data_list = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delete_database->root_folder_name,
			 list_display(
				delete_database->
					root_primary_attribute_data_list ) );
		exit( 1 );
	}

/*
printf( "%s\n", delete_database_display( delete_database ) );
*/

	delete_database_execute_delete_folder_list(
			delete_database->application_name,
			delete_database->delete_folder_list,
			"cat" /* sql_executable */ );

} /* test_benthic_appaserver_user() */

void test_shef( void )
{
	DELETE_DATABASE *delete_database;
	char *application_name = "hydrology";
	char *root_folder_name = "shef";
	char *primary_attribute_data_list_string = "HH";
	char delimiter = DELETE_DATABASE_DATA_DELIMITER;

	delete_database = 
		delete_database_new(
			application_name,
			0 /* not dont_delete_mto1_isa */,
			root_folder_name,
			primary_attribute_data_list_string,
			delimiter );

	if ( ! ( delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			delete_database->delete_folder_list,
			application_name,
			delete_database->dont_delete_mto1_isa,
			delete_database->root_folder_name,
			delete_database->root_primary_attribute_data_list ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: for folder = (%s), have empty delete_folder_list for root_primary_attribute_data_list = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delete_database->root_folder_name,
			 list_display(
				delete_database->
					root_primary_attribute_data_list ) );
		exit( 1 );
	}

/*
printf( "%s\n", delete_database_display( delete_database ) ); 
*/
	delete_database_execute_delete_folder_list(
			delete_database->application_name,
			delete_database->delete_folder_list,
			"cat" /* sql_executable  */ );

} /* test_shef() */

void test_capitolpops_musician( char *full_name )
{
	DELETE_DATABASE *delete_database;
	char *application_name = "capitolpops";
	char *root_folder_name = "musician";
	char *primary_attribute_data_list_string = full_name;
	char delimiter = DELETE_DATABASE_DATA_DELIMITER;

	delete_database = 
		delete_database_new(
			application_name,
			0 /* not dont_delete_mto1_isa */,
			root_folder_name,
			primary_attribute_data_list_string,
			delimiter );

	if ( ! ( delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			delete_database->delete_folder_list,
			application_name,
			delete_database->dont_delete_mto1_isa,
			delete_database->root_folder_name,
			delete_database->root_primary_attribute_data_list ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: for folder = (%s), have empty delete_folder_list for root_primary_attribute_data_list = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delete_database->root_folder_name,
			 list_display(
				delete_database->
					root_primary_attribute_data_list ) );
		exit( 1 );
	}

/*
printf( "%s\n", delete_database_display( delete_database ) );
*/

	delete_database_execute_delete_folder_list(
			delete_database->application_name,
			delete_database->delete_folder_list,
			"cat" /* sql_executable */ );

} /* test_capitolpops_musician() */

void test_capitolpops_band_member( boolean dont_delete_mto1_isa )
{
	DELETE_DATABASE *delete_database;
	char *application_name = "capitolpops";
	char *root_folder_name = "band_member";
	char *primary_attribute_data_list_string = "Tim Riley";
	char delimiter = DELETE_DATABASE_DATA_DELIMITER;

	delete_database = 
		delete_database_new(
			application_name,
			dont_delete_mto1_isa,
			root_folder_name,
			primary_attribute_data_list_string,
			delimiter );

	if ( ! ( delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			delete_database->delete_folder_list,
			application_name,
			delete_database->dont_delete_mto1_isa,
			delete_database->root_folder_name,
			delete_database->root_primary_attribute_data_list ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: for folder = (%s), have empty delete_folder_list for root_primary_attribute_data_list = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delete_database->root_folder_name,
			 list_display(
				delete_database->
					root_primary_attribute_data_list ) );
		exit( 1 );
	}

/*
printf( "%s\n", delete_database_display( delete_database ) );
*/

	delete_database_execute_delete_folder_list(
			delete_database->application_name,
			delete_database->delete_folder_list,
			"cat" /* sql_executable */ );

} /* test_capitolpops_band_member() */

void test_capitolpops_section( void )
{
	DELETE_DATABASE *delete_database;
	char *application_name = "capitolpops";
	char *root_folder_name = "section";
	char *primary_attribute_data_list_string = "Trombone";
	char delimiter = DELETE_DATABASE_DATA_DELIMITER;

	delete_database = 
		delete_database_new(
			application_name,
			0 /* not dont_delete_mto1_isa */,
			root_folder_name,
			primary_attribute_data_list_string,
			delimiter );

	if ( ! ( delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			delete_database->delete_folder_list,
			application_name,
			delete_database->dont_delete_mto1_isa,
			delete_database->root_folder_name,
			delete_database->root_primary_attribute_data_list ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: for folder = (%s), have empty delete_folder_list for root_primary_attribute_data_list = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delete_database->root_folder_name,
			 list_display(
				delete_database->
					root_primary_attribute_data_list ) );
		exit( 1 );
	}

	delete_database_execute_delete_folder_list(
			delete_database->application_name,
			delete_database->delete_folder_list,
			"cat" /* sql_executable */ );

} /* test_capitolpops_section() */

