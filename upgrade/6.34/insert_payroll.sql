delete from self;
insert into self (full_name,street_address,inventory_cost_method,payroll_pay_period,payroll_beginning_day,social_security_combined_tax_rate,social_security_payroll_ceiling,medicare_combined_tax_rate,medicare_additional_withholding_rate,medicare_additional_gross_pay_floor,federal_withholding_allowance_period_value,federal_nonresident_withholding_income_premium,state_withholding_allowance_period_value,state_itemized_allowance_period_value,federal_unemployment_wage_base,federal_unemployment_tax_standard_rate,federal_unemployment_threshold_rate,federal_unemployment_tax_minimum_rate,state_unemployment_wage_base,state_unemployment_tax_rate,state_sales_tax_rate) values ('Donner Mine Music Camp, Inc.','P.O. 906','average','weekly','friday','0.1240','127200','0.0290',null,null,'77.90',null,null,'19.50','7000','0.0600','0.0540','0.0060','7000','0.0340','0.0850');
delete from inventory_cost_method;
insert into inventory_cost_method (inventory_cost_method) values ('average');
insert into inventory_cost_method (inventory_cost_method) values ('FIFO');
insert into inventory_cost_method (inventory_cost_method) values ('LIFO');
delete from day;
insert into day (day) values ('friday');
insert into day (day) values ('monday');
insert into day (day) values ('saturday');
insert into day (day) values ('sunday');
insert into day (day) values ('thursday');
insert into day (day) values ('tuesday');
insert into day (day) values ('wednesday');
delete from payroll_pay_period;
insert into payroll_pay_period (payroll_pay_period) values ('biweekly');
insert into payroll_pay_period (payroll_pay_period) values ('monthly');
insert into payroll_pay_period (payroll_pay_period) values ('semimonthly');
insert into payroll_pay_period (payroll_pay_period) values ('weekly');
delete from federal_income_tax_withholding;
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single','44','224','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single','224','774','18.00','15.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single','774','1812','100.50','25.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single','1812','3730','360.00','28.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single','3730','8058','897.04','33.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single','8058','8090','2325.28','35.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single','8090','0','2336.48','39.6');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_but_single_rate','44','224','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_but_single_rate','224','774','18.00','15.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_but_single_rate','774','1812','100.50','25.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_but_single_rate','1812','3730','360.00','28.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_but_single_rate','3730','8058','897.04','33.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_but_single_rate','8058','8090','2325.28','35.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_but_single_rate','8090','0','2336.48','39.6');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married','166','525','0.00','10.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married','525','1626','35.90','15.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married','1626','3111','201.05','25.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married','3111','4654','572.30','28.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married','4654','8180','1004.34','33.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married','8180','9218','2167.92','35.0');
insert into federal_income_tax_withholding (federal_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married','9218','0','2531.22','39.6');
delete from federal_marital_status;
insert into federal_marital_status (federal_marital_status) values ('married');
insert into federal_marital_status (federal_marital_status) values ('married_but_single_rate');
insert into federal_marital_status (federal_marital_status) values ('single');
delete from state_marital_status;
insert into state_marital_status (state_marital_status) values ('married_one_income');
insert into state_marital_status (state_marital_status) values ('single_or_married_with_multiple_incomes');
insert into state_marital_status (state_marital_status) values ('unmarried_head_of_household');
delete from state_standard_deduction_table;
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','0','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','1','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','2','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','3','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','4','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','5','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','6','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','7','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','8','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','9','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('single_or_married_with_multiple_incomes','10','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','0','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','1','79.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','2','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','3','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','4','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','5','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','6','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','7','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','8','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','9','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('married_one_income','10','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','0','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','1','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','2','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','3','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','4','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','5','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','6','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','7','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','8','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','9','159.00');
insert into state_standard_deduction_table (state_marital_status,state_withholding_allowances,state_standard_deduction_amount) values ('unmarried_head_of_household','10','159.00');
delete from state_income_tax_withholding;
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','0','154','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','154','365','1.69','2.0');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','354','577','6.33','4.4');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','577','801','15.66','6.6');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','801','1012','30.44','8.8');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','1012','5168','49.01','10.2');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','5169','6202','474.17','11.3');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','6202','10337','591.32','12.4');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','10337','19231','1105.30','13.5');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('single_or_married_with_multiple_incomes','19231',null,'2308.66','14.6');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','0','308','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','308','730','3.39','2.2');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','730','1154','12.67','4.4');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','1154','1602','31.33','6.6');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','1602','2024','60.90','8.8');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','2024','10336','98.04','10.2');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','10336','12404','948.69','11.3');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','12404','19231','1182.66','12.4');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','19231','20673','2031.26','13.5');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('married_one_income','20673','0','2226.36','14.6');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','0','308','0.00','1.1');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','308','731','3.39','2.2');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','731','942','12.70','4.4');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','942','1166','21.98','6.6');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','1166','1377','36.76','8.8');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','1377','7029','55.33','10.2');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','7029','8435','633.53','11.3');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','8435','14058','792.83','12.4');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','14058','19231','1491.77','13.5');
insert into state_income_tax_withholding (state_marital_status,income_over,income_not_over,tax_fixed_amount,tax_percentage_amount) values ('unmarried_head_of_household','19231','0','2191.68','14.6');
