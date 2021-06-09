#include <clashdomeach.hpp>

void clashdomeach::claim(name account, uint8_t id, uint64_t asset_id, uint16_t game_id) {

    require_auth(account);

    atomicassets::assets_t player_assets = atomicassets::get_assets(account);
    auto asset_itr = player_assets.require_find(asset_id, "No NFT with this ID exists");

    // TODO: ELIMINATE HARDCODED PARAMETERS

    // check that the assets corresponds to our collection
    check(asset_itr->collection_name == name("clashdomenft"), "NFT doesn't correspond to clashdomenft");
    check(asset_itr->schema_name == name("gamedatatest"), "NFT doesn't correspond to schema gamedata");
    check(asset_itr->template_id == 91070, "NFT doesn't correspond to template  #91070");

    // check that the achievement id is in the correct range
    check(id > 0 && id < 16, "achievement id: " + to_string(id) + " is out of range");

    atomicassets::schemas_t collection_schemas = atomicassets::get_schemas(name("clashdomenft"));
    auto schema_itr = collection_schemas.find(name("gamedatatest").value);

    atomicassets::templates_t collection_templates = atomicassets::get_templates(name("clashdomenft"));
    auto template_itr = collection_templates.find(91070);

    vector <uint8_t> immutable_serialized_data = template_itr->immutable_serialized_data;
    vector <uint8_t> mutable_serialized_data = asset_itr->mutable_serialized_data;

    atomicassets::ATTRIBUTE_MAP idata = atomicdata::deserialize(immutable_serialized_data, schema_itr->format);
    atomicassets::ATTRIBUTE_MAP mdata = atomicdata::deserialize(mutable_serialized_data, schema_itr->format);
    
    string achievements_definition_str = get<string>(idata["achievements_definition"]);
    string achievements_str = get<string>(mdata["achievements"]);

    auto achievements_definition = json::parse(achievements_definition_str);
    auto achievements = json::parse(achievements_str);

    auto achievement_threshold_values = achievements_definition["achievement" + to_string(id)]["values"];
    string achievement_name = achievements_definition["achievement" + to_string(id)]["name"];

    int c = achievements["a" + to_string(id)]["c"];

    check(c < 6, "achievement id: " + to_string(id) + ", " + achievement_name +  "has been already completed");

    int n = achievements["a" + to_string(id)]["n"];

    int threshold_value = achievement_threshold_values[c];

    check(n == threshold_value, "achievement id: " + to_string(id) + ", " + achievement_name +  " can't be claimed");

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

    auto rewards_itr = ludiorewards.find(game_id);

    // TODO: HACER UN CHECK DE rewards_itr != ludiorewards.end();

    uint64_t pos = finder(rewards_itr->values, id);
    uint32_t ludio_achievement_reward = rewards_itr_>values[pos].ludios[c];

    asset ludio;
    ludio.symbol = LUDIO_SYMBOL;
    ludio.amount = ludio_achievement_reward * 10000;

    c ++;

    achievements["a" + to_string(id)]["c"] = c;

    mdata["achievements"] = achievements.dump();

    action(
        permission_level{get_self(), name("active")},
        name("clashdometkn"),
        name("transfer"),
        std::make_tuple (
            get_self(),
            account,
            ludio,
            "Candy Fiesta achievement " + to_string(id) + " unlocked"
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