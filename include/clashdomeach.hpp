#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <atomicassets.hpp>
#include <atomicdata.hpp>

#include <nlohmann/json.hpp>

using namespace eosio;
using namespace std;

using json = nlohmann::json;

#define ATOMIC name("atomicassets")
#define EOSIO name("eosio")

#define CONTRACTN name("clashdomeach")

CONTRACT clashdomeach : public contract {

public:

    using contract::contract;

    ACTION claim(name account, uint8_t id, uint64_t asset_id, uint16_t game_id);
    ACTION setrewards(uint16_t game_id, uint16_t index, vector<uint32_t> values);

private:

    struct values_s {
        uint16_t id;
        vector<uint32_t> ludios;
    };

    TABLE ludiorewards_s {
        uint16_t game_id;
        vector <values_s> values;

        uint64_t primary_key() const {return (uint64_t) game_id;}
    };

    typedef multi_index <name("ludiorewards"), ludiorewards_s> ludiorewards_t;

    ludiorewards_t ludiorewards = ludiorewards_t(get_self(), get_self().value);

    static constexpr symbol LUDIO_SYMBOL = symbol(symbol_code("LUDIO"), 4);

    uint64_t finder(vector<values_s> values, uint16_t id);
};