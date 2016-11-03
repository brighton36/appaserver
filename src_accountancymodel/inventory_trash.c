double inventory_get_on_hand_average_unit_cost(
				char *application_name,
				char *inventory_name )
{
	INVENTORY *inventory;

	inventory = inventory_new( inventory_name );

	inventory_load(		&inventory->retail_price,
				&inventory->reorder_quantity,
				&inventory->
					last_inventory_balance->
					on_hand_quantity,
				&inventory->
					last_inventory_balance->
					on_hand_average_unit_cost,
				&inventory->
					last_inventory_balance->
					on_hand_average_total_cost,
				application_name,
				inventory->inventory_name );

	return inventory->last_inventory_balance->on_hand_average_unit_cost;

} /* inventory_get_on_hand_average_unit_cost() */

void inventory_reset_on_hand_average_variables(
				char *application_name,
				char *inventory_name )
{
	INVENTORY_BALANCE *inventory_balance;

	inventory_balance = inventory_balance_new();

	inventory_last_inventory_balance_update(
				inventory_balance,
				application_name,
				inventory_name );

} /* inventory_reset_on_hand_average_variables() */

