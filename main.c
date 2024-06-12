#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bank.h"

int main(int argc, char **argv) {
    srand(time(NULL));
    Banker bank;
    int num_customers = random_number_between(MIN_NB_CUSTOMERS, MAX_NB_CUSTOMERS);
    Customer customers[num_customers];
    
    rdm_customer_resources(customers, num_customers);
    find_ideal_resources_for_banker(customers, num_customers, &bank);
    reset_deny_count(&bank);
    
    int safe_state = 1;
    
    while(safe_state) {
        rdm_customer_requests(customers, num_customers);
        reset_deny_count(&bank);
        
        for (id = 0; id < num_customers && safe_state; id++) {
            update_safe_states(customers, num_customers, &bank);
            
            for (res_id = 0; res_id < RES_TYPE; res_id++) {
                if (!banker_grant_request(customers[id], res_id, bank)) {
                    bank.deny_count[res_id]++;
                    
                    if (bank.deny_count[res_id] >= num_customers) {
                        printf("Simulation did %d transactions", safe_state - 1);
                        printf(" before reaching an unsafe state.\n\n");
                        safe_state = 0;
                        break;
                    }
                } else {
                    process_transfer(&customers[id], res_id, &bank);
                    safe_state++;
                }
                
                if (safe_state > NB_MAX_OF_SIMULATIONS) {
                    printf("\nSimulation did %d transactions", safe_state - 1);
                    printf(" without encountering any unsafe state.\n");
                    safe_state = 0;
                    break;
                }
            }
        }
    }
    
    report_customers(customers, num_customers);
    report_banker(bank);
    
    return 0;
}
