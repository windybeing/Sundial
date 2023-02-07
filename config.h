#pragma once

// number of server threads on each node
#define NUM_SERVER_THREADS 6            // same as paper, confirmed

// CPU_FREQ is used to get accurate timing info
// We assume all nodes have the same CPU frequency.
#define CPU_FREQ 2.5     // in GHz/s    // confirmed

// warmup time in seconds
#define WARMUP 0
// WORKLOAD can be YCSB or TPCC
#define WORKLOAD YCSB10
#define SIMULATE_REMOTE false

// Statistics
// ==========
// COLLECT_LATENCY: when set to true, will collect transaction latency information
#define COLLECT_LATENCY false           // statics related, disabled
// PRT_LAT_DISTR: when set to true, will print transaction latency distribution
#define PRT_LAT_DISTR false             // statics related, disabled
#define STATS_ENABLE true               // statics related, must be enabled
#define TIME_ENABLE true                // statics related, must be enabled
#define STATS_CP_INTERVAL 1000          // in ms, one seconrd, confirmed

// Concurrency Control
// ===================
// Supported concurrency control algorithms: WAIT_DIE, NO_WAIT, TICTOC, F_ONE, MAAT
#define CC_ALG TICTOC                   // checked to be TICTOC
#define ISOLATION_LEVEL SERIALIZABLE    // confirmed

// KEY_ORDER: when set to true, each transaction accesses tuples in the primary key order.
// Only effective when WORKLOAD == YCSB.
#define KEY_ORDER false

// per-row lock/ts management or central lock/ts management
#define BUCKET_CNT 31
#define MAX_NUM_ABORTS 0
#define ABORT_PENALTY 1000000
#define ABORT_BUFFER_SIZE 10        // unused
#define ABORT_BUFFER_ENABLE true    // unused
// [ INDEX ]
#define ENABLE_LATCH false          // unused, only for INDEX_BTREE
#define CENTRAL_INDEX false         // unused
#define CENTRAL_MANAGER false       // unused
#define INDEX_STRUCT IDX_HASH       // only INDEX_HASH is available, INDEX_BTREE introduces compile error
#define BTREE_ORDER 16              // unused, only for INDEX_BTREE

// [Two Phase Locking]
#define NO_LOCK false // NO_LOCK=true : used to model H-Store
// [TIMESTAMP]
#define TS_ALLOC TS_CLOCK
#define TS_BATCH_ALLOC false
#define TS_BATCH_NUM 1
// [MVCC]
#define MIN_TS_INTVL 5000000 //5 ms. In nanoseconds
// [OCC]
#define MAX_WRITE_SET 10
#define PER_ROW_VALID true
// [TICTOC]
#define WRITE_COPY_FORM "data" // ptr or data   // unused
#define TICTOC_MV false                         // unused as ATOMIC_WORD is not used
#define WR_VALIDATION_SEPARATE true             // unused
#define WRITE_PERMISSION_LOCK false             // unused as ATOMIC_WORD is not used
#define ATOMIC_TIMESTAMP "false"                // unused

// when WAW_LOCK is true, lock a tuple before write.
// essentially, WW conflicts are handled as 2PL.
#define OCC_WAW_LOCK                true        // same as paper
// if SKIP_READONLY_PREPARE is true, then a readonly subtxn will forget
// about its states after returning. If no renewal is required, this remote
// node will not participate in the 2PC protocol.
#define SKIP_READONLY_PREPARE        false                  // unused
#define MAX_NUM_WAITS                4
#define READ_INTENSITY_THRESH         0.8                   // same as paper

// [Caching in TicToc]
#define ENABLE_LOCAL_CACHING         true                   // same as paper
#define CACHING_POLICY                READ_INTENSIVE        // same as paper
#define RO_LEASE                    false   // XXX:tunable
#define LOCAL_CACHE_SIZE            (1024*1024) // in KB    // same as paper
#define REUSE_FRESH_DATA            false
#define REUSE_IF_NO_REMOTE             false

#define LOCK_ALL_BEFORE_COMMIT        false // unrelated: Lock read set
#define LOCK_ALL_DEBUG                false // unrelated: related to print
#define TRACK_LAST                  false
#define LOCK_TRIAL                    3     // unused
#define MULTI_VERSION               false
// [TICTOC, SILO]
#define OCC_LOCK_TYPE                 WAIT_DIE
#define PRE_ABORT                    true
#define ATOMIC_WORD                    false    // XXX: compile error
#define UPDATE_TABLE_TS                true
// [MAAT]
#define DEBUG_REFCOUNT                false     // debug
// [HSTORE]
// when set to true, hstore will not access the global timestamp.
// This is fine for single partition transactions.
#define HSTORE_LOCAL_TS                false    // unused
// [VLL]
#define TXN_QUEUE_SIZE_LIMIT        THREAD_CNT  // unused

////////////////////////////////////////////////////////////////////////
// Logging  // disabled by default, if enabled, performance will drop
////////////////////////////////////////////////////////////////////////
#define LOG_ENABLE                    true
#define LOG_COMMAND                    false
#define LOG_REDO                    false
#define LOG_BATCH_TIME                10 // in ms

////////////////////////////////////////////////////////////////////////
// Benchmark
////////////////////////////////////////////////////////////////////////
// max number of rows touched per transaction
#define WARMUP_TIME                 0 // in seconds     // cmd parameter
#define RUN_TIME                    1 // in seconds     // cmd parameter
#define MAX_TUPLE_SIZE              1024 // in bytes    // unused
#define INIT_PARALLELISM            40                  // unrelated
///////////////////////////////
// YCSB
///////////////////////////////
// Number of tuples per node
#define SYNTH_TABLE_SIZE             (1024 * 1024 * 10)                     // XXX: a wrong configuration
#define ZIPF_THETA                     0.9                                  // the same as paper
#define READ_PERC                     0.9                                   // the same as paper
#define PERC_READONLY_DATA            0                                     // unused
#define PERC_REMOTE                    0.1                                  // the same as paper
#define SINGLE_PART_ONLY            false // access single partition only   // the same as paper
#define REQ_PER_QUERY                16                                     // the same as paper
#define THINK_TIME                    0  // in us                           // not mentioned in the paper
#define SOCIAL_NETWORK                false                                 // not mentioned in the paper
///////////////////////////////
// TPCC // unrelated to YCSB
///////////////////////////////

// For large warehouse count, the tables do not fit in memory
// small tpcc schemas shrink the table size.
#define TPCC_SMALL false
#define NUM_WH 5
// TODO. REPLICATE_ITEM_TABLE = false only works for TICTOC.
#define REPLICATE_ITEM_TABLE        true

#define PERC_NEWORDER                 0.45
#define PERC_PAYMENT                 0.43
#define PERC_ORDERSTATUS            0.04
#define PERC_DELIVERY                0.04
#define PERC_STOCKLEVEL                0.04
#define PAYMENT_REMOTE_PERC            15 // 15% customers are remote
#define NEW_ORDER_REMOTE_PERC        20  // 1% order lines are remote
#define FIRSTNAME_MINLEN             8
#define FIRSTNAME_LEN                 16
#define LASTNAME_LEN                 16
#define DIST_PER_WARE                10


#define REMOTE_TPCC false
#define NORMAL_DELIVERY false

///////////////////////////////
// TATP // unrelated to YCSB
///////////////////////////////
// Number of subscribers per node.
#define TATP_POPULATION                100000

////////////////////////////////////////////////////////////////////////
// TODO centralized CC management.
////////////////////////////////////////////////////////////////////////
#define MAX_LOCK_CNT                (20 * THREAD_CNT)   // unused
#define TSTAB_SIZE                  50 * THREAD_CNT     // unused
#define TSTAB_FREE                  TSTAB_SIZE          // unused
#define TSREQ_FREE                  4 * TSTAB_FREE      // unused
#define MVHIS_FREE                  4 * TSTAB_FREE      // unused
#define SPIN                        false               // unused

////////////////////////////////////////////////////////////////////////
// Test cases
////////////////////////////////////////////////////////////////////////
#define TEST_ALL                    true        // unused
enum TestCases {
    READ_WRITE,
    CONFLICT
};
extern TestCases                    g_test_case;

////////////////////////////////////////////////////////////////////////
// DEBUG info
////////////////////////////////////////////////////////////////////////
#define WL_VERB                        true // unused
#define IDX_VERB                    false   // unused
#define VERB_ALLOC                    true  // unused

#define DEBUG_LOCK                    false     // debug related
#define DEBUG_TIMESTAMP                false    // debug related
#define DEBUG_SYNTH                    false    // debug related
#define DEBUG_ASSERT                false       // debug related
#define DEBUG_CC                    false       // debug related

////////////////////////////////////////////////////////////////////////
// Constant // enumeration
////////////////////////////////////////////////////////////////////////
// index structure
#define IDX_HASH                     1
#define IDX_BTREE                    2
// WORKLOAD
#define YCSB                        1
#define TPCC                        2
#define YCSB10                      3
// Concurrency Control Algorithm
#define NO_WAIT                        1
#define WAIT_DIE                    2
#define F_ONE                        3
#define MAAT                        4
#define IDEAL_MVCC                     5
#define NAIVE_TICTOC                6
#define TICTOC                        7
#define TCM                            8
//Isolation Levels
#define SERIALIZABLE                1
#define SNAPSHOT                    2
#define REPEATABLE_READ                3
// TIMESTAMP allocation method.
#define TS_MUTEX                    1
#define TS_CAS                        2
#define TS_HW                        3
#define TS_CLOCK                    4
// Commit protocol
#define TWO_PHASE_COMMIT            1
#define OWNERSHIP                    2
// Caching policy
#define ALWAYS_READ                    1    // always read cached data
#define ALWAYS_CHECK                2    // always contact remote node
#define READ_INTENSIVE                3    // only read cached data that is read-intensive

/***********************************************/
// Distributed DBMS
/***********************************************/
#define START_PORT 35777            // unrelated
#define INOUT_QUEUE_SIZE 1024  // XXX: checked but do not influence performance
#define NUM_INPUT_THREADS 1         // same to paper
#define NUM_OUTPUT_THREADS 1        // same to paper
#define MAX_NUM_ACTIVE_TXNS 128     // 128 has already saturate one server
#define ENABLE_MSG_BUFFER false  // XXX:hurt performance
#define MAX_MESSAGE_SIZE 16384
#define RECV_BUFFER_SIZE 32768
#define SEND_BUFFER_SIZE 32768
#define MODEL_DUMMY_MSG false       // unused

#define MAX_CLOCK_SKEW 0 // in us   // unused
