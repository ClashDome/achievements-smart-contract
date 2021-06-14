#include <clashdomeach.hpp>

void clashdomeach::claimludio(name account, uint8_t id, uint64_t asset_id, uint16_t game_id) {

    require_auth(account);

    // CHECK IF game_id IS VALID
    check(game_id >= 0 && game_id < sizeof(GAME_NAMES)/sizeof(GAME_NAMES[0]), "wrong game ID:" + to_string(game_id));

    atomicassets::assets_t player_assets = atomicassets::get_assets(account);
    auto asset_itr = player_assets.require_find(asset_id, "No NFT with this ID exists");

    uint32_t template_id = TEMPLATE_ID[game_id];

    // CHECK THAT THE ASSET CORRESPONDS TO OUR COLLECTION
    check(asset_itr->collection_name == name(COLLECTION_NAME), "NFT doesn't correspond to " + COLLECTION_NAME);
    check(asset_itr->schema_name == name(SCHEMA_NAME), "NFT doesn't correspond to schema " + SCHEMA_NAME);
    check(asset_itr->template_id == template_id, "NFT doesn't correspond to template #" + to_string(template_id));

    // GET NFT INMUTABLE AND MUTABLE DATA
    atomicassets::schemas_t collection_schemas = atomicassets::get_schemas(name(COLLECTION_NAME));
    auto schema_itr = collection_schemas.find(name(SCHEMA_NAME).value);

    atomicassets::templates_t collection_templates = atomicassets::get_templates(name(COLLECTION_NAME));
    auto template_itr = collection_templates.find(TEMPLATE_ID[game_id]);

    vector <uint8_t> immutable_serialized_data = template_itr->immutable_serialized_data;
    vector <uint8_t> mutable_serialized_data = asset_itr->mutable_serialized_data;

    atomicassets::ATTRIBUTE_MAP idata = atomicdata::deserialize(immutable_serialized_data, schema_itr->format);
    atomicassets::ATTRIBUTE_MAP mdata = atomicdata::deserialize(mutable_serialized_data, schema_itr->format);
    
    string achievements_definition_str = get<string>(idata["achievements_definition"]);
    string achievements_str = get<string>(mdata["achievements"]);

    // PARSE THE CORRESPONDNG JSON OBJECTS
    auto achievements_definition = json::parse(achievements_definition_str);
    auto achievements = json::parse(achievements_str);

    // CHECK THAT THE ACHIEVEMENT ID IS IN THE CORRECT RANGE
    uint16_t i = 0;
    for (json::iterator it = achievements_definition.begin(); it != achievements_definition.end(); ++it) {
        i ++;
    }

    check(id > 0 && id < i + 1, "achievement with id: " + to_string(id) + " is out of range");

    auto achievement_threshold_values = achievements_definition["achievement" + to_string(id)]["values"];
    string achievement_name = achievements_definition["achievement" + to_string(id)]["name"];

    int c = achievements["a" + to_string(id)]["c"];

    check(c < 6, "achievement with id: " + to_string(id) + ", " + achievement_name +  "has been already completed");

    int n = achievements["a" + to_string(id)]["n"];

    int threshold_value = achievement_threshold_values[c];

    check(n == threshold_value, "achievement id: " + to_string(id) + ", " + achievement_name +  " can't be claimed");

    auto rewards_itr = ludiorewards.find(game_id);

    check(rewards_itr != ludiorewards.end(), "LUDIO rewards table not defined for game id:" + to_string(game_id));

    uint64_t pos = finder(rewards_itr->values, id);
    uint32_t ludio_achievement_reward = rewards_itr->values[pos].ludios[c];

    asset ludio;
    ludio.symbol = LUDIO_SYMBOL;
    ludio.amount = ludio_achievement_reward * 10000;

    action(
        permission_level{get_self(), name("active")},
        name("clashdometkn"),
        name("transfer"),
        std::make_tuple (
            get_self(),
            account,
            ludio,
            GAME_NAMES[game_id] + " achievement #" + to_string(id) + ", " + achievement_name +  " value:" + to_string(threshold_value) +  ", unlocked"
        )
    ).send();

    // WE INCREASE THE CLAIMED ACHIEVEMENT COUNTER
    c ++;

    achievements["a" + to_string(id)]["c"] = c;

    mdata["achievements"] = achievements.dump();

    // AND SAVE THE MUTABLE DATA IN THE NFT
    action(
        permission_level{get_self(), name("active")},
        name("atomicassets"),
        name("setassetdata"),
        std::make_tuple (
            get_self(),
            account,
            asset_id,
            mdata
        )
    ).send();
}

void clashdomeach::setrewards(uint16_t game_id, uint16_t index, vector<uint32_t> values) {

    require_auth(get_self());

    auto rewards_itr = ludiorewards.find(game_id);

    if (rewards_itr == ludiorewards.end()) {

        vector <values_s> values_struct;
        values_struct.push_back({index, values});
          
        ludiorewards.emplace(get_self(), [&](auto &_rewards) {
            _rewards.game_id = game_id;
            _rewards.values = values_struct;
        });

    } else {

        uint64_t pos = finder(rewards_itr->values, index);

        if (pos != -1) {

            ludiorewards.modify(rewards_itr, get_self(), [&](auto &_rewards) {

                _rewards.values[pos].ludios = values;
            });

        } else {
            
            ludiorewards.modify(rewards_itr, get_self(), [&](auto &_rewards) {

                _rewards.values.push_back({index, values});
            });
        }
    }
}

uint64_t clashdomeach::finder(vector<values_s> values, uint16_t id) {

    for (uint64_t i = 0; i < values.size(); i++) {

        if (values.at(i).id == id) {
            return i;
        }
    }
    return -1;
}