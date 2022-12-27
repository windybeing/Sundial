#pragma once

#include "global.h"
#include "helper.h"
#include "query.h"

class workload;

struct RequestYCSB10 {
public:
    access_t rtype;
    uint64_t key;
    uint32_t value;
};

class QueryYCSB10 : public QueryBase {
public:
    static void calculateDenom();

    QueryYCSB10();
    QueryYCSB10(char * raw_data);
    QueryYCSB10(RequestYCSB10 * requests, uint32_t num_requests);
    ~QueryYCSB10();

    uint32_t serialize(char * &raw_data);

    uint64_t get_request_count()     { return _request_cnt; }
    RequestYCSB10 * get_requests()    { return _requests; }
    void gen_requests();
    bool is_all_remote_readonly() { return _is_all_remote_readonly; }

private:
    uint32_t _request_cnt;
    RequestYCSB10 * _requests;

    // for Zipfian distribution
    uint64_t zipf(uint64_t n, double theta);
    static double zeta(uint64_t n, double theta);
    static uint64_t the_n;
    static double denom;
    static double zeta_2_theta;
    bool _is_all_remote_readonly;
};
