#pragma once

#include "evmc.h"
#include "hex_helpers.h"

#include "../stc/cstr.h"

/// Extended value (by dirty flag) for account storage.
struct storage_value
{
    /// The storage value.
    evmc_bytes32 value;

    /// True means this value has been modified already by the current transaction.
    bool dirty;

    /// Is the storage key cold or warm.
    enum evmc_access_status access_status;
};

/// Default constructor.
void storage_value_init(struct storage_value *sv)
{
    memset(sv->value.bytes, 0, 32);
    sv->dirty = false;
    sv->access_status = EVMC_ACCESS_COLD;
}

/// Constructor.
void storage_value_init_vd(struct storage_value *sv, evmc_bytes32 value, bool dirty)
{
    memcpy(sv->value.bytes, value.bytes, 32);
    sv->dirty = dirty;
    sv->access_status = EVMC_ACCESS_COLD;
}

/// Constructor with initial access status.
void storage_value_init_vs(struct storage_value *sv, evmc_bytes32 value, enum evmc_access_status as)
{
    memcpy(sv->value.bytes, value.bytes, 32);
    sv->dirty = false;
    sv->access_status = as;
}

// --------------------------------------------------------------------------------------------
// storage<evmc_bytes32, struct storage_value>
static inline int evmc_bytes32_cmp(const evmc_bytes32* a, const evmc_bytes32* b) {
    cstr tmpa = cstr_from_n((const char*)a->bytes, 32);
    cstr tmpb = cstr_from_n((const char*)b->bytes, 32);
    return cstr_cmp(&tmpa, &tmpb);
}

static inline uint32_t evmc_bytes32_hash(const evmc_bytes32* a) {
    cstr tmpa = cstr_from_n((const char*)a->bytes, 32);
    return cstr_hash(&tmpa);
}

static inline evmc_bytes32 evmc_bytes32_clone(evmc_bytes32 v) {
    cstr tmpv = cstr_from_n((const char*)v.bytes, 32);
    tmpv = cstr_clone(tmpv); 
    return v;
}

static inline void evmc_bytes32_drop(evmc_bytes32* vk) {
    cstr tmpvk = cstr_from_n((const char*)vk->bytes, 32);
    cstr_drop(&tmpvk);
}

#define i_type storage_map
#define i_key evmc_bytes32
#define i_cmp evmc_bytes32_cmp
#define i_hash evmc_bytes32_hash
#define i_keyclone evmc_bytes32_clone
#define i_keydrop evmc_bytes32_drop
#define i_val struct storage_value
#include "../stc/cmap.h"

/// Mocked account.
struct MockedAccount
{
    /// The account nonce.
    int nonce;

    /// The account code.
    cstr code;

    /// The code hash. Can be a value not related to the actual code.
    evmc_bytes32 codehash;

    /// The account balance.
    evmc_uint256be balance;

    /// The account storage map.
    storage_map storage;
};

/// Constructor of MockedAccount
void MockedAccount_init(struct MockedAccount* ma)
{
    ma->nonce = 0;
    ma->code = cstr_init();
    memset(ma->codehash.bytes, 0, 32);
    memset(ma->balance.bytes, 0, 32);
    ma->storage = storage_map_init();
}

/// Helper method for setting balance by numeric type.
void MockedAccount_set_balance(struct MockedAccount* ma, uint64_t x)
{
    memset(ma->balance.bytes, 0, 32);
    for(int i = 0; i < 8; ++i){
        ma->balance.bytes[32 - 1 - i] = (uint8_t)(x >> (8 * i));
    }
}

// --------------------------------------------------------------------------------------------
#define i_val evmc_bytes32
#define i_cmp evmc_bytes32_cmp
#define i_valdrop evmc_bytes32_drop
#define i_valclone evmc_bytes32_clone
#define i_tag b32
#include "../stc/cvec.h"

/// LOG record.
struct log_record
{
    /// The address of the account which created the log.
    evmc_address creator;

    /// The data attached to the log.
    cstr data;

    /// The log topics.
    cvec_b32 topics;
};

/// Constructor of log_record
void log_record_init(struct log_record* lr){
    memset(lr->creator.bytes, 0, 20);
    lr->data = cstr_init();
    lr->topics = cvec_b32_init();
}

/// Equal operator.
bool log_record_equal(struct log_record* lra, struct log_record* lrb){
    cstr tmp_addr_a = cstr_from_n((const char*)lra->creator.bytes, 20);
    cstr tmp_addr_b = cstr_from_n((const char*)lrb->creator.bytes, 20);
    bool res = cstr_equals(tmp_addr_a, cstr_str(&tmp_addr_b)) && cstr_equals(lra->data, cstr_str(&lrb->data));
    if(res){
        int size = cvec_b32_size(lra->topics);
        if(size != cvec_b32_size(lrb->topics)){
            return false;
        }
        else{
            for(int i = 0; i < size; i++){
                if(!evmc_bytes32_cmp(cvec_b32_at(&lra->topics, i), cvec_b32_at(&lrb->topics, i))){
                    return false;
                }
            }
            return true;
        }
    }
    else{
        return false;
    }
}

// --------------------------------------------------------------------------------------------
/// SELFDESTRUCT record.
struct selfdestruct_record
{
    /// The address of the account which has self-destructed.
    evmc_address selfdestructed;

    /// The address of the beneficiary account.
    evmc_address beneficiary;
};

/// Constructor of selfdestruct_record
void selfdestruct_record_init(struct selfdestruct_record* sr){
    memset(sr->selfdestructed.bytes, 0, 20);
    memset(sr->beneficiary.bytes, 0, 20);
}

/// Equal operator.
bool selfdestructed_record_equal(struct selfdestruct_record* sra, struct selfdestruct_record* srb)
{
    cstr tmp_selfa = cstr_from_n((const char*)sra->selfdestructed.bytes, 20);
    cstr tmp_selfb = cstr_from_n((const char*)srb->selfdestructed.bytes, 20);

    cstr tmp_benea = cstr_from_n((const char*)sra->beneficiary.bytes, 20);
    cstr tmp_beneb = cstr_from_n((const char*)srb->beneficiary.bytes, 20);

    return cstr_equals(tmp_selfa, cstr_str(&tmp_selfb)) && cstr_equals(tmp_benea, cstr_str(&tmp_beneb));
}

// --------------------------------------------------------------------------------------------
// storage<evmc_address, struct MockedAccount>

static inline int evmc_address_cmp(const evmc_address* a, const evmc_address* b) {
    cstr tmpa = cstr_from_n((const char*)a->bytes, 20);
    cstr tmpb = cstr_from_n((const char*)b->bytes, 20);
    return cstr_cmp(&tmpa, &tmpb);
}

static inline uint32_t evmc_address_hash(const evmc_address* a) {
    cstr tmpa = cstr_from_n((const char*)a->bytes, 20);
    return cstr_hash(&tmpa);
}

static inline evmc_address evmc_address_clone(evmc_address v) {
    cstr tmpv = cstr_from_n((const char*)v.bytes, 20);
    tmpv = cstr_clone(tmpv); 
    return v;
}

static inline void evmc_address_drop(evmc_address* vk) {
    cstr tmpvk = cstr_from_n((const char*)vk->bytes, 20);
    cstr_drop(&tmpvk);
}

#define i_type account_map
#define i_key evmc_address
#define i_cmp evmc_address_cmp
#define i_hash evmc_address_hash
#define i_keyclone evmc_address_clone
#define i_keydrop evmc_address_drop
#define i_val struct MockedAccount
#include "../stc/cmap.h"

#define i_val int64_t
#define i_tag int64
#include "../stc/cvec.h"

#define i_val evmc_address
#define i_cmp evmc_address_cmp
#define i_valclone evmc_address_clone
#define i_valdrop evmc_address_drop
#define i_tag addr
#include "../stc/cvec.h"

static inline int evmc_message_cmp(const struct evmc_message* a, const struct evmc_message* b) {
    cstr tmpa = cstr_from_n((const char*)a->input_data, a->input_size);
    cstr tmpb = cstr_from_n((const char*)b->input_data, b->input_size);
    return cstr_cmp(&tmpa, &tmpb);
}

static inline struct evmc_message evmc_message_clone(struct evmc_message v) {
    cstr tmpv = cstr_from_n((const char*)v.input_data, v.input_size);
    tmpv = cstr_clone(tmpv); 
    return v;
}

static inline void evmc_message_drop(struct evmc_message* vk) {
    cstr tmpvk = cstr_from_n((const char*)vk->input_data, vk->input_size);
    cstr_drop(&tmpvk);
}

#define i_val struct evmc_message
#define i_cmp evmc_message_cmp
#define i_valdrop evmc_message_drop
#define i_valclone evmc_message_clone
#define i_tag message
#include "../stc/cvec.h"

static inline int log_record_cmp(const struct log_record* a, const struct log_record* b) {
    int c = evmc_address_cmp(&a->creator, &b->creator);
    if(c)
        return c;
    else{
        int d = cstr_cmp(&a->data, &b->data);
        if(d)
            return d;
        else{
            int sizea = cvec_b32_size(a->topics);
            int sizeb = cvec_b32_size(b->topics);
            if(sizea != sizeb){
                return sizea - sizeb;
            }
            else{
                return 0;
            }
        }
    }
}

static inline struct log_record log_record_clone(struct log_record v) {
    v.creator = evmc_address_clone(v.creator);
    v.data = cstr_clone(v.data);
    for(int i = 0; i < cvec_b32_size(v.topics); i++){
        *cvec_b32_at_mut(&v.topics, i) = evmc_bytes32_clone(*cvec_b32_at_mut(&v.topics, i));
    }
    return v;
}

static inline void log_record_drop(struct log_record* v) {
    evmc_address_drop(&v->creator);
    cstr_drop(&v->data);
    for(int i = 0; i < cvec_b32_size(v->topics); i++){
        evmc_bytes32_drop(cvec_b32_at_mut(&v->topics, i));
    }
}

#define i_val struct log_record
#define i_cmp log_record_cmp
#define i_valdrop log_record_drop
#define i_valclone log_record_clone
#define i_tag logrecord
#include "../stc/cvec.h"

static inline int selfdestruct_record_cmp(const struct selfdestruct_record* a, const struct selfdestruct_record* b) {
    int c = evmc_address_cmp(&a->selfdestructed, &b->selfdestructed);
    return c ? c : evmc_address_cmp(&a->beneficiary, &b->beneficiary);
}

static inline struct selfdestruct_record selfdestruct_record_clone(struct selfdestruct_record v) {
    v.selfdestructed = evmc_address_clone(v.selfdestructed);
    v.beneficiary = evmc_address_clone(v.beneficiary);
    return v;
}

static inline void selfdestruct_record_drop(struct selfdestruct_record* v) {
    evmc_address_drop(&v->selfdestructed);
    evmc_address_drop(&v->beneficiary);
}

#define i_val struct selfdestruct_record
#define i_cmp selfdestruct_record_cmp
#define i_valdrop selfdestruct_record_drop
#define i_valclone selfdestruct_record_clone
#define i_tag selfdestructedrecord
#include "../stc/cvec.h"

#define i_val_str
#define i_tag bytes
#include "../stc/cvec.h"

typedef struct evmc_host_context
{   
    /// LOG record.
    struct log_record lr;

    /// SELFDESTRUCT record.
    struct selfdestruct_record sr;

    /// The set of all accounts in the Host, organized by their addresses.
    account_map accounts;

    /// The EVM transaction context to be returned by get_tx_context().
    struct evmc_tx_context tx_context;

    /// The block header hash value to be returned by get_block_hash().
    evmc_bytes32 block_hash;

    /// The call result to be returned by the call() method.
    struct evmc_result call_result;

    /// The record of all block numbers for which get_block_hash() was called.
    cvec_int64 recorded_blockhashes;

    /// The record of all account accesses.
    cvec_addr recorded_account_accesses;

    /// The maximum number of entries in recorded_account_accesses record.
    /// This is arbitrary value useful in fuzzing when we don't want the record to explode.
    size_t max_recorded_account_accesses;

    /// The record of all call messages requested in the call() method.
    cvec_message recorded_calls;

    /// The maximum number of entries in recorded_calls record.
    /// This is arbitrary value useful in fuzzing when we don't want the record to explode.
    size_t max_recorded_calls;

    /// The record of all LOGs passed to the emit_log() method.
    cvec_logrecord recorded_logs;

    /// The record of all SELFDESTRUCTs from the selfdestruct() method.
    cvec_selfdestructedrecord recorded_selfdestructs;

    /// The copy of call inputs for the recorded_calls record.
    cvec_bytes m_recorded_calls_inputs;
}MockedHost;

// Constructor of MockedAccount
void MockedHost_init(struct evmc_host_context *mockhost)
{   
    log_record_init(&mockhost->lr);
    selfdestruct_record_init(&mockhost->sr);
    mockhost->accounts = account_map_init();
    evmcContext_init(mockhost->tx_context);
    memset(mockhost->block_hash.bytes, 0, 32);
    evmc_result_init(&mockhost->call_result);
    mockhost->recorded_blockhashes = cvec_int64_init();

    mockhost->recorded_account_accesses = cvec_addr_init();
    mockhost->max_recorded_account_accesses = 200;
    
    mockhost->recorded_calls = cvec_message_init();
    mockhost->max_recorded_calls = 100;

    mockhost->recorded_logs = cvec_logrecord_init();
    mockhost->recorded_selfdestructs = cvec_selfdestructedrecord_init();
    mockhost->m_recorded_calls_inputs = cvec_bytes_init();
}

/// Record an account access.
/// @param addr  The address of the accessed account.
void record_account_access(MockedHost *mockhost, evmc_address addr)
{   
    // mockhost->recorded_account_accesses = cvec_addr_init();
    if (cvec_addr_empty(mockhost->recorded_account_accesses))
        cvec_addr_reserve(&(mockhost->recorded_account_accesses), mockhost->max_recorded_account_accesses);
    if (cvec_addr_size(mockhost->recorded_account_accesses) < mockhost->max_recorded_account_accesses)
        cvec_addr_push(&(mockhost->recorded_account_accesses), addr);
}

/// Returns true if an account exists (EVMC Host method).
bool account_exists(MockedHost *mockhost, evmc_address addr)
{
    record_account_access(mockhost, addr);
    return account_map_contains(&(mockhost->accounts), addr);
}

/// Get the account's storage value at the given key (EVMC Host method).
evmc_bytes32 get_storage(MockedHost *mockhost, evmc_address addr, evmc_bytes32 key)
{
    record_account_access(mockhost, addr);
    evmc_bytes32 res;
    memset(res.bytes, 0, 32);

    account_map_iter account_iter = account_map_find(&(mockhost->accounts), addr); 
    if (account_iter.ref == account_map_end(&(mockhost->accounts)).ref)
        return res;

    storage_map_iter storage_iter = storage_map_find(&account_iter.ref->second.storage, key);
    if(storage_iter.ref != storage_map_end(&account_iter.ref->second.storage).ref)
        return storage_iter.ref->second.value;
    return res;
}

/// Set the account's storage value (EVMC Host method).
enum evmc_storage_status set_storage(MockedHost *mockhost, evmc_address addr, evmc_bytes32 key, evmc_bytes32 value)
{
    record_account_access(mockhost, addr);

    // Get the reference to the old value.
    // This will create the account in case it was not present.
    // This is convenient for unit testing and standalone EVM execution to preserve the
    // storage values after the execution terminates.
    struct MockedAccount* old_acc = &account_map_get_mut(&(mockhost->accounts), addr)->second;
    struct storage_value* old = &storage_map_get_mut(&old_acc->storage, key)->second;

    // Follow https://eips.ethereum.org/EIPS/eip-1283 specification.
    // WARNING! This is not complete implementation as refund is not handled here.
    if (evmc_bytes32_cmp(&old->value, &value) == 0)
        return EVMC_STORAGE_UNCHANGED;

    enum evmc_storage_status status;
    if (!old->dirty)
    {
        old->dirty = true;
        if (b32_zero(old->value))
            status = EVMC_STORAGE_ADDED;
        else if (!b32_zero(value))
            status = EVMC_STORAGE_MODIFIED;
        else
            status = EVMC_STORAGE_DELETED;
    }
    else
        status = EVMC_STORAGE_MODIFIED_AGAIN;

    memcpy(old->value.bytes, value.bytes, 32);
    return status;
}

/// Get the account's balance (EVMC Host method).
evmc_uint256be get_balance(MockedHost *mockhost, evmc_address addr)
{
    record_account_access(mockhost, addr);
    account_map_iter it = account_map_find(&mockhost->accounts, addr);
    if(it.ref == account_map_end(&mockhost->accounts).ref){
        evmc_uint256be res;
        memset(res.bytes, 0, 32);
        return res;
    }
    return it.ref->second.balance;
}

/// Get the account's code size (EVMC host method).
size_t get_code_size(MockedHost *mockhost, evmc_address addr)
{
    record_account_access(mockhost, addr);
    account_map_iter it = account_map_find(&mockhost->accounts, addr);
    if(it.ref == account_map_end(&mockhost->accounts).ref){
        return 0;
    }
    return cstr_size(it.ref->second.code);
}

/// Get the account's code hash (EVMC host method).
evmc_bytes32 get_code_hash(MockedHost *mockhost, evmc_address addr)
{
    record_account_access(mockhost, addr);
    account_map_iter it = account_map_find(&mockhost->accounts, addr);
    if(it.ref == account_map_end(&mockhost->accounts).ref){
        evmc_uint256be res;
        memset(res.bytes, 0, 32);
        return res;
    }
    return it.ref->second.codehash;
}

// Copy the account's code to the given buffer (EVMC host method).
size_t copy_code(MockedHost *mockhost, evmc_address addr, size_t code_offset, uint8_t *buffer_data, size_t buffer_size)
{
    record_account_access(mockhost, addr);

    account_map_iter it = account_map_find(&mockhost->accounts, addr);
    if(it.ref == account_map_end(&mockhost->accounts).ref)
        return 0;

    cstr code = cstr_clone(it.ref->second.code);

    if(code_offset >= cstr_size(code))
        return 0;

    size_t n = MIN(buffer_size, cstr_size(code)-code_offset);

    if(n > 0)
        cstr_replace_at(&code, code_offset, n, (char *)buffer_data);
    return n;
}

/// Selfdestruct the account (EVMC host method).
void selfdestruct(MockedHost *mockhost, evmc_address addr, evmc_address beneficiary)
{
    record_account_access(mockhost, addr);

    struct selfdestruct_record sr;
    sr.selfdestructed = addr;
    sr.beneficiary = beneficiary;
    cvec_selfdestructedrecord_push_back(&(mockhost->recorded_selfdestructs), sr);
}

/// Call/create other contract (EVMC host method).
struct evmc_result call(MockedHost *mockhost, struct evmc_message msg)
{
    record_account_access(mockhost, msg.recipient);

    if (cvec_message_empty(mockhost->recorded_calls))
    {
        cvec_message_reserve(&(mockhost->recorded_calls), mockhost->max_recorded_calls);
        // Iterators will not invalidate.
        cvec_bytes_reserve(&(mockhost->m_recorded_calls_inputs), mockhost->max_recorded_calls);
    }

    if (cvec_message_size(mockhost->recorded_calls) < mockhost->max_recorded_calls)
    {
        cvec_message_push(&(mockhost->recorded_calls), msg);
        struct evmc_message *call_msg = cvec_message_back(&(mockhost->recorded_calls));
        if (call_msg->input_size > 0)
        {
            cstr input = cstr_from_n(call_msg->input_data, call_msg->input_size);
            cvec_bytes_push(&(mockhost->m_recorded_calls_inputs), input);

            cstr* input_copy = cvec_bytes_back(&(mockhost->m_recorded_calls_inputs));
            call_msg->input_data = (uint8_t *)cstr_data(input_copy);
        }
    }
    return mockhost->call_result;
}

/// Get transaction context (EVMC host method).
struct evmc_tx_context get_tx_context(MockedHost *mockhost)
{
    return mockhost->tx_context;
}

/// Get the block header hash (EVMC host method).
evmc_bytes32 get_block_hash(MockedHost *mockhost, int64_t block_number)
{
    cvec_int64_push(&(mockhost->recorded_blockhashes), block_number);
    return mockhost->block_hash;
}

// Emit LOG (EVMC host method).
void emit_log(MockedHost *mockhost, evmc_address addr, uint8_t *data, size_t data_size, evmc_bytes32 topics[], size_t topics_count)
{
    struct log_record lr;
    lr.creator = addr;
    lr.data = cstr_from_n(data, data_size);
    for(int i = 0; i < topics_count; i++){
        cvec_b32_push(&lr.topics, topics[i]);
    }
    cvec_logrecord_push_back(&(mockhost->recorded_logs), lr);
}

/// Record an account access.
///
/// This method is required by EIP-2929 introduced in ::EVMC_BERLIN. It will record the account
/// access in MockedHost::recorded_account_accesses and return previous access status.
/// This methods returns ::EVMC_ACCESS_WARM for known addresses of precompiles.
/// The EIP-2929 specifies that evmc_message::sender and evmc_message::recipient are always
/// ::EVMC_ACCESS_WARM. Therefore, you should init the MockedHost with:
///
///     mocked_host.access_account(msg.sender);
///     mocked_host.access_account(msg.recipient);
///
/// The same way you can mock transaction access list (EIP-2930) for account addresses.
///
/// @param addr  The address of the accessed account.
/// @returns     The ::EVMC_ACCESS_WARM if the account has been accessed before,
///              the ::EVMC_ACCESS_COLD otherwise.
enum evmc_access_status access_account(MockedHost *mockhost, evmc_address addr)
{
    // Check if the address have been already accessed.
    bool already_accessed = false;
    cvec_addr_iter it = cvec_addr_find(&mockhost->recorded_account_accesses, addr);
    if(it.ref != cvec_addr_end(&mockhost->recorded_account_accesses).ref)
        already_accessed = true;

    record_account_access(mockhost, addr);

    evmc_address addr1 = u64_to_addr((uint64_t)1);
    evmc_address addr9 = u64_to_addr((uint64_t)9);

    // Accessing precompiled contracts is always warm.
    if(evmc_address_cmp(&addr, &addr1) >= 0 && evmc_address_cmp(&addr, &addr9) <= 0)
        return EVMC_ACCESS_WARM;

    return already_accessed ? EVMC_ACCESS_WARM : EVMC_ACCESS_COLD;
}

/// Access the account's storage value at the given key.
///
/// This method is required by EIP-2929 introduced in ::EVMC_BERLIN. In records that the given
/// account's storage key has been access and returns the previous access status.
/// To mock storage access list (EIP-2930), you can pre-init account's storage values with
/// the ::EVMC_ACCESS_WARM flag:
///
///     mocked_host.accounts[msg.recipient].storage[key] = {value, EVMC_ACCESS_WARM};
///
/// @param addr  The account address.
/// @param key   The account's storage key.
/// @return      The ::EVMC_ACCESS_WARM if the storage key has been accessed before,
///              the ::EVMC_ACCESS_COLD otherwise.
enum evmc_access_status access_storage(MockedHost *mockhost, evmc_address addr, evmc_bytes32 key)
{
    enum evmc_access_status access_status;
    if(account_map_empty(mockhost->accounts) || !account_map_contains(&(mockhost->accounts), addr)){
        struct MockedAccount acc;
        MockedAccount_init(&acc);
        account_map_insert(&mockhost->accounts, addr, acc);
    }
    struct MockedAccount* acc = &account_map_get_mut(&(mockhost->accounts), addr)->second;
    if(storage_map_empty(acc->storage) || !storage_map_contains(&(acc->storage), key)){
        struct storage_value sv;
        storage_value_init(&sv);
        storage_map_insert(&acc->storage, key, sv);
    }
    struct storage_value svalue = storage_map_get_mut(&(acc->storage), key)->second;
    access_status = svalue.access_status;
    svalue.access_status = EVMC_ACCESS_WARM;
    storage_map_insert_or_assign(&(acc->storage), key, svalue);
    return access_status;
}