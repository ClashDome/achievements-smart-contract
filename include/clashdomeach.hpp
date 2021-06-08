#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <atomicassets.hpp>
#include <atomicdata.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace eosio;
using namespace std;

using namespace rapidjson;

#define ATOMIC name("atomicassets")
#define EOSIO name("eosio")

#define CONTRACTN name("clashdomeach")

CONTRACT clashdomeach : public contract {

public:

    using contract::contract;

    ACTION claim(name account, uint8_t id, uint64_t asset_id);
    ACTION logclaim(atomicassets::ATTRIBUTE_MAP mdata);

private:

    TABLE ludiorewards{
        uint8_t id;
        vector <uint16_t> values;
    };
};