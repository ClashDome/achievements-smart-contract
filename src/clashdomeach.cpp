#include <clashdomeach.hpp>

void clashdomeach::claim(name account, uint8_t id, uint64_t asset_id) {

    require_auth(account);

    atomicassets::assets_t player_assets = atomicassets::get_assets(account);
    auto asset_itr = player_assets.require_find(asset_id, "No NFT with this ID exists");

    // comprobar que el asset es de la coleccion que toca
    check(asset_itr->collection_name == name("clashdomenft"), "NFT doesn't correspond to clashdomenft");
    check(asset_itr->schema_name == name("gamedatatest"), "NFT doesn't correspond to schema gamedata");
    check(asset_itr->template_id == 91070, "NFT doesn't correspond to template  #91070");

    atomicassets::schemas_t collection_schemas = atomicassets::get_schemas(name("clashdomenft"));
    auto schema_itr = collection_schemas.find(name("gamedatatest").value);

    vector <uint8_t> inmutable_serialized_data = asset_itr->immutable_serialized_data;
    vector <uint8_t> mutable_serialized_data = asset_itr->mutable_serialized_data;

    // TODO: deserializar estos 2 vectores
    atomicassets::ATTRIBUTE_MAP mdata = atomicdata::deserialize(mutable_serialized_data, schema_itr->format);

    auto achievements_json = mdata["achievements"];
     
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    mdata["achievements"] = string(buffer.GetString());

    action(
        permission_level{get_self(), name("active")},
        get_self(),
        name("logclaim"),
        std::make_tuple (
            mdata
        )
    ).send();
}

void clashdomeach::logclaim( atomicassets::ATTRIBUTE_MAP mdata) {

    require_auth(get_self());
}
