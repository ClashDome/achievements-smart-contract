project(clashdomeach)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( clashdomeach clashdomeach clashdomeach.cpp )
target_include_directories( clashdomeach PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( clashdomeach ${CMAKE_SOURCE_DIR}/../ricardian )