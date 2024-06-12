#define RES_TYPE 4
#define NB_MAX_OF_SIMULATIONS 202
#define MAX_NB_CUSTOMERS 12
#define MIN_NB_CUSTOMERS 5
#define MAX_RESOURCE_LIMIT 12
#define MIN_RESOURCE_LIMIT 5
#define MAX_REQUEST 5
#define MIN_REQUEST 0

int id, res, res_id;

int random_number_between(const int min, const int max) {
    return min + (rand() % (max - min + 1));
}

typedef struct {
    int total_resources[RES_TYPE];
    int stock[RES_TYPE];
    int safe_state[RES_TYPE];
    int deny_count[RES_TYPE];
} Banker;

typedef struct {
    int max_resources[RES_TYPE];
    int stock[RES_TYPE];
    int request[RES_TYPE];
} Customer;

void report_customer(const Customer cust) {
    for (res = 0; res < RES_TYPE; res++) {
        printf("\t%d?%d/%d", cust.request[res], cust.stock[res], cust.max_resources[res]);
    }
    printf("\n");
}

void report_customers(const Customer *customers, const int num_customers) {
    printf("Customers");
    for (res_id = 0; res_id < RES_TYPE; res_id++) {
        printf("\tres%d", res_id);
    }
    printf("\n");
    for (id = 0; id < num_customers; id++) {
        printf("#%d\t", id);
        report_customer(customers[id]);
    }
}

void rdm_customer_resources(Customer *customers, const int num_customers) {
    for (id = 0; id < num_customers; id++) {
        for (res_id = 0; res_id < RES_TYPE; res_id++) {
            customers[id].max_resources[res_id] = random_number_between(MIN_RESOURCE_LIMIT, MAX_RESOURCE_LIMIT);
            customers[id].stock[res_id] = 0;
            customers[id].request[res_id] = 0;
        }
    }
}

void rdm_customer_requests(Customer *customers, const int num_customers) {
    for (id = 0; id < num_customers; id++) {
        for (res_id = 0; res_id < RES_TYPE; res_id++) {
            customers[id].request[res_id] = random_number_between(MIN_REQUEST, MAX_REQUEST);
        }
    }
}

void report_banker(const Banker banker) {
    printf("Banker\nStock:\t");
    for (res = 0; res < RES_TYPE; res++) {
        printf("\t%d", banker.stock[res]);
    }
    printf("\nTotal:\t");
    for (res = 0; res < RES_TYPE; res++) {
        printf("\t%d", banker.total_resources[res]);
    }
    printf("\nSafe:\t");
    for (res = 0; res < RES_TYPE; res++) {
        printf("\t%d", banker.safe_state[res]);
    }
    printf("\nDeny:\t");
    for (res = 0; res < RES_TYPE; res++) {
        printf("\t%d", banker.deny_count[res]);
    }
    printf("\n");
}

void find_ideal_resources_for_banker(const Customer *customers, const int num_customers, Banker *banker) {
    for (res = 0; res < RES_TYPE; res++) {
        banker->total_resources[res] = 0;
        for (id = 0; id < num_customers; id++) {
            banker->total_resources[res] += customers[id].max_resources[res];
        }
        banker->stock[res] = (3 * banker->total_resources[res] / 5);
        banker->safe_state[res] = -1;
    }
}

void update_safe_states(const Customer *customers, const int num_customers, Banker *banker) {
    int remaining;
    for (res = 0; res < RES_TYPE; res++) {
        banker->safe_state[res] = banker->total_resources[res];
        for (id = 0; id < num_customers; id++) {
            remaining = customers[id].max_resources[res] - customers[id].stock[res];
            if (banker->safe_state[res] > remaining) {
                if (remaining < 0) {
                    remaining = 0;
                }
                banker->safe_state[res] = remaining;
            }
        }
    }
}

int banker_grant_request(Customer customer, const int res_id, Banker banker) {
    int bank_after = banker.stock[res_id] - customer.request[res_id];
    int customer_after = customer.stock[res_id] + customer.request[res_id];
    if (customer_after <= customer.max_resources[res_id]) {
        if (bank_after >= 0) {
            if (bank_after >= banker.safe_state[res_id]) {
                return 1;
            }
        }
    }
    return 0;
}

void process_transfer(Customer *customer, const int res_id, Banker *banker) {
    banker->stock[res_id] -= customer->request[res_id];
    customer->stock[res_id] += customer->request[res_id];
    customer->request[res_id] = 0;
    if (customer->stock[res_id] >= customer->max_resources[res_id]) {
        banker->stock[res_id] += customer->stock[res_id];
        customer->stock[res_id] = 0;
    }
}

void reset_deny_count(Banker *banker) {
    for (res_id = 0; res_id < RES_TYPE; res_id++) {
        banker->deny_count[res_id] = 0;
    }
}
