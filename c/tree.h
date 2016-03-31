#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <stdbool.h>
#include <stdint.h>
#include <uthash.h>

#ifdef INT64_T
#define HAVE_INT64
#endif

#ifdef __INT64
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#define HAVE_INT64
#endif

#ifdef INT64_DI
typedef int int64_t __attribute__ ((__mode__(DI)));
typedef unsigned int uint64_t __attribute__ ((__mode__(DTI)));
#define HAVE_INT64
#endif

#ifndef HAVE_INT64
#error "No int64 type define was passed to the compiler!"
#endif

#ifdef INT128_TI
typedef int int128_t __attribute__ ((__mode__(TI)));
typedef unsigned int uint128_t __attribute__ ((__mode__(TI)));
#define HAVE_INT128
#endif

#ifdef __INT128
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;
#define HAVE_INT128
#endif

#ifndef HAVE_INT128
#error "No int128 type define was passed to the compiler!"
#endif

/* perl memory allocator does not guarantee 16-byte alignment */
typedef int128_t int128_t_a8 __attribute__ ((aligned(8)));
typedef uint128_t uint128_t_a8 __attribute__ ((aligned(8)));

#define MATH_INT64_NATIVE_IF_AVAILABLE
#include "perl_math_int64.h"
#include "perl_math_int128.h"

typedef enum {
    MMDBW_RECORD_TYPE_EMPTY,
    MMDBW_RECORD_TYPE_DATA,
    MMDBW_RECORD_TYPE_NODE,
    MMDBW_RECORD_TYPE_ALIAS,
} MMDBW_record_type;

typedef enum {
    MMDBW_MERGE_STRATEGY_NONE,
    MMDBW_MERGE_STRATEGY_TOPLEVEL,
    MMDBW_MERGE_STRATEGY_RECURSE
} MMDBW_merge_strategy;

typedef struct MMDBW_record_s {
    union {
        const char *key;
        struct MMDBW_node_s *node;
    } value;
    MMDBW_record_type type;
} MMDBW_record_s;

typedef struct MMDBW_node_s {
    MMDBW_record_s left_record;
    MMDBW_record_s right_record;
    uint32_t number;
} MMDBW_node_s;

typedef struct MMDBW_data_hash_s {
    SV *data_sv;
    const char *key;
    uint32_t reference_count;
    UT_hash_handle hh;
} MMDBW_data_hash_s;

typedef struct MMDBW_tree_s {
    uint8_t ip_version;
    uint8_t record_size;
    MMDBW_merge_strategy merge_strategy;
    MMDBW_data_hash_s *data_table;
    MMDBW_node_s *root_node;
    uint32_t node_count;
    bool is_finalized;
    bool is_aliased;
} MMDBW_tree_s;

typedef struct MMDBW_network_s {
    const uint8_t *const bytes;
    const uint8_t prefix_length;
} MMDBW_network_s;

    /* *INDENT-OFF* */
    /* --prototypes automatically generated by dev-bin/regen-prototypes.pl - don't remove this comment */
    extern MMDBW_tree_s *new_tree(const uint8_t ip_version, uint8_t record_size,
                                  MMDBW_merge_strategy merge_strategy,
                                  const bool alias_ipv6);
    extern void insert_network(MMDBW_tree_s *tree, const char *ipstr,
                               const uint8_t prefix_length, SV *key_sv, SV *data,
                               bool force_overwrite);
    extern void insert_range(MMDBW_tree_s *tree, const char *start_ipstr,
                             const char *end_ipstr, SV *key_sv, SV *data_sv,
                             bool force_overwrite);
    extern void remove_network(MMDBW_tree_s *tree, const char *ipstr,
                               const uint8_t prefix_length);
    extern SV *merge_hashes_for_keys(MMDBW_tree_s *tree, const char *const key_from,
                                     const char *const key_into, MMDBW_network_s *network);
    extern SV *lookup_ip_address(MMDBW_tree_s *tree, const char *const ipstr);
    extern MMDBW_node_s *new_node(MMDBW_tree_s *tree);
    extern void finalize_tree(MMDBW_tree_s *tree);
    extern void freeze_tree(MMDBW_tree_s *tree, char *filename, char *frozen_params,
                            size_t frozen_params_size);
    extern MMDBW_tree_s *thaw_tree(char *filename, uint32_t initial_offset,
                                   uint8_t ip_version, uint8_t record_size,
                                   MMDBW_merge_strategy merge_strategy,
                                   const bool alias_ipv6);
    extern void write_search_tree(MMDBW_tree_s *tree, SV *output,
                                  SV *root_data_type, SV *serializer);
    extern uint32_t max_record_value(MMDBW_tree_s *tree);
    extern void start_iteration(MMDBW_tree_s *tree,
                                bool depth_first,
                                void *args,
                                void(callback) (MMDBW_tree_s *tree,
                                                MMDBW_node_s *node,
                                                uint128_t network,
                                                uint8_t depth,
                                                void *args));
    extern uint128_t flip_network_bit(MMDBW_tree_s *tree, uint128_t network, uint8_t depth);
    extern SV *data_for_key(MMDBW_tree_s *tree, const char *const key);
    extern void free_tree(MMDBW_tree_s *tree);
    extern const char *record_type_name(int record_type);
    extern void warn_hex(uint8_t digest[16], char *where);
    extern char *md5_as_hex(uint8_t digest[16]);
    /* --prototypes end - don't remove this comment-- */
    /* *INDENT-ON* */
