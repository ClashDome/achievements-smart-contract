#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <atomicassets.hpp>
#include <atomicdata.hpp>

using namespace eosio;
using namespace std;

#define ATOMIC name("atomicassets")
#define EOSIO name("eosio")

#define CONTRACTN name("clashdomeach")

CONTRACT clashdomeach : public contract
{
public:
   using contract::contract;

    ACTION actiontest(
        uint64_t param1,
        uint64_t param2
    );

private:

    TABLE test_table_s {
        uint64_t pack_id;
        name     collection_name;
        uint32_t unlock_time;
        int32_t  pack_template_id;

        uint64_t primary_key() const { return pack_id; }
        uint64_t by_template_id() const { return (uint64_t) pack_template_id; };
    };

    typedef multi_index<name("testtable"), test_table_s,
        indexed_by < name("templateid"), const_mem_fun < test_table_s, uint64_t, &test_table_s::by_template_id>>>
    test_table_t;

    test_table_t             test_table           = test_table_t(get_self(), get_self().value);

};