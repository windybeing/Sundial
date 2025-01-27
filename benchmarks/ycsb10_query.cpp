#include "query.h"
#include "ycsb10_query.h"
#include "workload.h"
#include "ycsb10.h"
#include "table.h"
#include "manager.h"

uint64_t QueryYCSB10::the_n = 0;
double QueryYCSB10::denom = 0;
double QueryYCSB10::zeta_2_theta;

void
QueryYCSB10::calculateDenom()
{
    assert(the_n == 0);
    uint64_t table_size = g_synth_table_size;
    #if SINGLE_PART_ONLY
    assert(table_size % g_num_worker_threads == 0);
    the_n = table_size / g_num_worker_threads - 1;
    #else
    the_n = table_size - 1;
    #endif
    denom = zeta(the_n, g_zipf_theta);
    zeta_2_theta = zeta(2, g_zipf_theta);
}

// The following algorithm comes from the paper:
// Quickly generating billion-record synthetic databases
// However, it seems there is a small bug.
// The original paper says zeta(theta, 2.0). But I guess it should be
// zeta(2.0, theta).
double QueryYCSB10::zeta(uint64_t n, double theta) {
    double sum = 0;
    for (uint64_t i = 1; i <= n; i++)
        sum += pow(1.0 / i, theta);
    return sum;
}

uint64_t QueryYCSB10::zipf(uint64_t n, double theta) {
    assert(this->the_n == n);
    assert(theta == g_zipf_theta);
    double alpha = 1 / (1 - theta);
    double zetan = denom;
    double eta = (1 - pow(2.0 / n, 1 - theta)) /
        (1 - zeta_2_theta / zetan);
    double u = glob_manager->rand_double();
    double uz = u * zetan;
    if (uz < 1) return 0;
    if (uz < 1 + pow(0.5, theta)) return 1;
    return (uint64_t)(n * pow(eta*u -eta + 1, alpha));
}


QueryYCSB10::QueryYCSB10()
    : QueryBase()
{
    _requests = NULL;
    _requests = (RequestYCSB10 *) MALLOC(sizeof(RequestYCSB10) * g_req_per_query);
    gen_requests();
    _is_all_remote_readonly = false;
}

QueryYCSB10::QueryYCSB10(char * raw_data)
{
    assert(false);
    //memcpy(this, raw_data, sizeof(*this));
    if (_request_cnt > 0) {
        _requests = (RequestYCSB10 *) MALLOC(sizeof(RequestYCSB10) * _request_cnt);
        memcpy(_requests, raw_data + sizeof(*this), sizeof(RequestYCSB10) * _request_cnt);
    }
}

QueryYCSB10::QueryYCSB10(RequestYCSB10 * requests, uint32_t num_requests)
{
    _request_cnt = num_requests;
    _requests = (RequestYCSB10 *) MALLOC(sizeof(RequestYCSB10) * _request_cnt);
    memcpy(_requests, requests, sizeof(RequestYCSB10) * _request_cnt);
}

QueryYCSB10::~QueryYCSB10()
{
    FREE(_requests, sizeof(RequestYCSB10) * _request_cnt);
}

void QueryYCSB10::gen_requests() {
    _request_cnt = 0;
    uint64_t all_keys[g_req_per_query];
    bool has_remote = false;
    _is_all_remote_readonly = true;
    // uint64_t table_size = g_synth_table_size;
    bool readonly = glob_manager->rand_double() < g_readonly_perc;
    for (uint32_t tmp = 0; tmp < g_req_per_query; tmp ++) {
        RequestYCSB10 * req = &_requests[_request_cnt];

        // bool remote = (g_num_nodes > 1)? (glob_manager->rand_double() < g_perc_remote) : false;
        // uint32_t node_id;
        // if (remote) {
        //     node_id = (g_node_id + glob_manager->rand_uint64(1, g_num_nodes - 1)) % g_num_nodes;
        //     has_remote = true;
        // } else
        //     node_id = g_node_id;
        #if SINGLE_PART_ONLY
        uint64_t row_id = zipf(table_size / g_num_worker_threads - 1, g_zipf_theta);
        row_id = row_id * g_num_worker_threads + GET_THD_ID;
        assert(row_id < table_size);
        #else
        // uint64_t row_id = zipf(table_size - 1, g_zipf_theta);
        #endif
        uint64_t primary_key;
        bool exist = false;
        do {
            exist = false;
            primary_key = zipfianGenerator->nextValue();
#ifdef ENABLE_DISTRIBUTED_TXN
            bool remote = (g_num_nodes > 1)? (glob_manager->rand_double() < g_perc_remote) : false;
            uint64_t node;
            if (remote && tmp == 0) {
                do {
                    node = glob_manager->rand_uint64(0, g_num_nodes - 1); 
                } while (node == g_node_id);
            } else {
                node = g_node_id;
            }
            primary_key = node + g_num_nodes * primary_key;
#endif
            // remove duplicates
            for (uint32_t i = 0; i < tmp; i++)
                if (all_keys[i] == primary_key)
                    exist = true;
            if (!exist)
                all_keys[_request_cnt ++] = primary_key;
        } while (exist);
        if (readonly)
            req->rtype = RD;
        else {
            double r = glob_manager->rand_double();
            req->rtype = (r < g_read_perc)? RD : WR;
        }
        if (req->rtype == WR && GET_WORKLOAD->key_to_node(primary_key) != g_node_id)
            _is_all_remote_readonly = false;

        #if SOCIAL_NETWORK
        // if this switch is turned on, we mimic a social network
        // where writing events happen uniformly while reading events
        // follows the power law.
        // Here we re-sample the row_id from a uniform distribution.
        if(req->rtype == WR)
        {
            // it is possible that the new write destination (row_id) is a read-only item.
            row_id = (uint64_t) ((table_size - 1) * glob_manager->rand_double());
            for(;;)
            {
            bool readonly = (row_id == 0)? false :
                (int(row_id * g_readonly_perc) > int((row_id - 1) * g_readonly_perc));
                if(!readonly) break;
                else { assert(false); row_id ++; } // avoid the read-only area
            }
            primary_key = row_id * g_num_server_nodes + node_id;
        }
        #endif

        req->key = primary_key;
        req->value = 0;
    }
    if (!has_remote)
        _is_all_remote_readonly = false;
    // Sort the requests in key order.
    if (g_key_order) {
        for (int i = _request_cnt - 1; i > 0; i--)
            for (int j = 0; j < i; j ++)
                if (_requests[j].key > _requests[j + 1].key) {
                    RequestYCSB10 tmp = _requests[j];
                    _requests[j] = _requests[j + 1];
                    _requests[j + 1] = tmp;
                }
        for (UInt32 i = 0; i < _request_cnt - 1; i++)
            assert(_requests[i].key < _requests[i + 1].key);
    }
}

uint32_t
QueryYCSB10::serialize(char * &raw_data)
{
    uint32_t size = sizeof(*this);
    size += _request_cnt * sizeof(RequestYCSB10);
    raw_data = (char *) MALLOC(size);
    memcpy(raw_data, this, sizeof(*this));
    memcpy(raw_data + sizeof(*this), _requests, _request_cnt * sizeof(RequestYCSB10));
    return size;
}
