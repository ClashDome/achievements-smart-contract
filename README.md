achievements-smart-contract

# Build

```cd <smart_contract_directory>
git clone
go to directory
md build
cd build
cmake ..
make
```

# Update contract

While testing in testnet:
cleos -u https://testnet.waxsweden.org set contract clashdomeach ./clashdomeach -p clashdomeach@active

In production:
cleos -u https://api.waxsweden.org set contract clashdomeach ./clashdomeach -p clashdomeach@active