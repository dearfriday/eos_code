#bash

rm -rf /home/friday/eosio-wallet/one1.wallet
./cleos wallet create -n one1 -f ./pwd.data
./cleos wallet import -n one1 --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3		##EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
./cleos wallet import -n one1 --private-key 5KfCYBBKPUgLEXyLTCEDh8CHFP78663Vv48KoQ2nrsktZPmgUWq		##EOS5NwmM6VgzZTTMVdAhbVYyNgoStW6n28MvTNS4qkph6ApARpd1H
./cleos wallet import -n one1 --private-key 5Hv4opnTKVsDJgsLyTxX8nXjy8XrDQpMRgV4FwKL1frReyJyHnF		##EOS5PdFrHsHEsTpABGeoGEPbh2A77oYo7rkcSHPAJDukZXX8VkiZa
./cleos wallet import -n one1 --private-key 5JpmixmaJXrjUynFE1HdbwKQ8XijPRRaGG87fxiL1bt7anqkLBd		##EOS6fDWAjxmuQmj1kgWz2VjThtAn2fUFZGsnatecMHCFpB13DkrRt
./cleos wallet import -n one1 --private-key 5JpesZ2H7H8miGPjhTNesNAt67176Ca9ZMNb5C2Vqt3iPj2kKYu		##EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM

##create test1
./cleos create account eosio test.coin EOS5NwmM6VgzZTTMVdAhbVYyNgoStW6n28MvTNS4qkph6ApARpd1H EOS5PdFrHsHEsTpABGeoGEPbh2A77oYo7rkcSHPAJDukZXX8VkiZa
./cleos create account eosio test.token EOS5NwmM6VgzZTTMVdAhbVYyNgoStW6n28MvTNS4qkph6ApARpd1H EOS5PdFrHsHEsTpABGeoGEPbh2A77oYo7rkcSHPAJDukZXX8VkiZa

./cleos create account eosio test1 EOS6fDWAjxmuQmj1kgWz2VjThtAn2fUFZGsnatecMHCFpB13DkrRt EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM
./cleos create account eosio test2 EOS6fDWAjxmuQmj1kgWz2VjThtAn2fUFZGsnatecMHCFpB13DkrRt EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM
./cleos create account eosio test3 EOS6fDWAjxmuQmj1kgWz2VjThtAn2fUFZGsnatecMHCFpB13DkrRt EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM
./cleos create account eosio test4 EOS6fDWAjxmuQmj1kgWz2VjThtAn2fUFZGsnatecMHCFpB13DkrRt EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM



#set contract
./cleos set contract test.token /home/friday/Documents/projects/eos/build/contracts/eosio.token eosio.token.wasm eosio.token.abi
./cleos set contract test.coin /home/friday/Documents/projects/eos/build/contracts/playcoin/ playcoin.wasm playcoin.abi
#create token
./cleos push action test.token create '["test.token", "1000000.0000 TST"]' -p test.token
#issue token
#./cleos push action test.token issue '["test1", "100.0000 TST", "init"]' -p test.token
./cleos push action test.token issue '["test1", "10000.0000 TST", "init"]' -p test.token
./cleos push action test.token issue '["test2", "10000.0000 TST", "init"]' -p test.token
./cleos push action test.token issue '["test3", "10000.0000 TST", "init"]' -p test.token
./cleos push action test.token issue '["test4", "10000.0000 TST", "init"]' -p test.token

#permission
./cleos set account permission test1 active '{"threshold": 1,"keys": [{"key": "EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM","weight": 1}],"accounts": [{"permission":{"actor":"test.coin","permission":"eosio.code"},"weight":1}]}' owner -p test1
./cleos set account permission test2 active '{"threshold": 1,"keys": [{"key": "EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM","weight": 1}],"accounts": [{"permission":{"actor":"test.coin","permission":"eosio.code"},"weight":1}]}' owner -p test2
./cleos set account permission test3 active '{"threshold": 1,"keys": [{"key": "EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM","weight": 1}],"accounts": [{"permission":{"actor":"test.coin","permission":"eosio.code"},"weight":1}]}' owner -p test3
./cleos set account permission test4 active '{"threshold": 1,"keys": [{"key": "EOS4vbn668HGuMoCKK4dFWqPvgK1FxHc5FfP5Rt7DtVXTk7RkPmwM","weight": 1}],"accounts": [{"permission":{"actor":"test.coin","permission":"eosio.code"},"weight":1}]}' owner -p test4

####push bet
./cleos push action test.coin deposit '["test1", "100.0000 TST"]' -p test1
./cleos push action test.coin pushbet  '[1, "1.0000 TST", "test1"]' -p test1

./cleos push action test.coin deposit '["test2", "100.0000 TST"]' -p test2
./cleos push action test.coin pushbet  '[0, "3.0000 TST", "test2"]' -p test2

./cleos push action test.coin deposit '["test3", "100.0000 TST"]' -p test3
./cleos push action test.coin pushbet  '[0, "1.0000 TST", "test3"]' -p test3

./cleos push action test.coin deposit '["test4", "100.0000 TST"]' -p test4
./cleos push action test.coin pushbet  '[1, "3.0000 TST", "test4"]' -p test4


#
#
#
#
#