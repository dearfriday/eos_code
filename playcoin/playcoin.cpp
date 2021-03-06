#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/transaction.h>

using eosio::action;
using eosio::asset;
using eosio::const_mem_fun;
using eosio::indexed_by;
using eosio::key256;
using eosio::name;
using eosio::permission_level;
using eosio::print;

class playcoin : public eosio::contract
{
  public:
    playcoin(account_name self)
        : eosio::contract(self)
          // , game(_self, _self),
          ,
          globalgame(_self, _self), accounts(_self, _self), offers(_self, _self)
    {
    }

    /// @abi action
    void pushbet(const uint32_t is_big, const asset &bet, const account_name player)
    {
        print("player ", name{player}, "  max ", is_big, "\n");
        bet.print();
        eosio_assert((is_big == 1 || is_big == 0), "is_big only 0 or 1");
        require_auth(player);
        auto player_itr = accounts.find(player);
        eosio_assert(player_itr != accounts.end(), "player not find.");
        eosio_assert(bet.amount >= 10000, "bet must > 1.0000");
        auto gl_game = globalgame.begin();
        if (gl_game == globalgame.end())
        {
            gl_game = globalgame.emplace(_self, [](auto &gl) {
                gl.nextgameid = 0;
            });
        }

        auto itr = offers.emplace(_self, [&](auto &obj) {
            obj.id         = offers.available_primary_key();
            obj.owner = player;
            obj.bet = bet;
            obj.max = is_big;
            obj.gameid = gl_game->nextgameid;
        });
        print("push offer ", name{itr->owner}, " bet ", itr->bet.amount, "\n");
    }

    /// @abi action
    void cancelbet(const account_name &player, const asset &bet)
    {
        bet.print();
        print("\ncancelbet  \n");
        print("player ", name{player}, "\n");
    }

    /// @abi action
    void lockgame(const account_name &player)
    {
        eosio_assert(player == _self, "only self can lockgame");
        auto gl_game = globalgame.begin();
        eosio_assert(gl_game != globalgame.end(), "global game not find. please offer");
        globalgame.modify(gl_game, 0, [&](auto &obj) {
            obj.islocked = true;
            obj.lockblocknum = tapos_block_num();
        });
        print("lock game num ", gl_game->lockblocknum);
    }

    // /// @abi action
    void openresult(const account_name &player)
    {
        eosio_assert(player == _self, "only self can open result");

        int rand = tapos_block_prefix() % 2;
        rand = rand > 0 ? rand : -rand;
        print("open result prefix num ", rand, "  ret ", rand % 2);

        //find global game
        auto gl_game = globalgame.begin();
        eosio_assert(gl_game != globalgame.end(), "global game not find. please offer");
        // globalgame.modify(gl_game, 0, [](auto &obj) {
        //     obj.islocked = true;
        //     obj.lockblocknum = tapos_block_num();
        // });

        auto itr = offers.template get_index<N(gameid)>();
        auto itr_lower = itr.lower_bound(gl_game->nextgameid);
        if (itr_lower == itr.end())
        {
            eosio_assert(false, "not find offer.");
        }
       
        uint32_t up = 0;
        uint32_t down = 0;
        std::map<account_name, uint32_t> up_all;
        std::map<account_name, uint32_t> down_all;
        account_name up_max_player;
        asset up_max_bet;
        account_name down_max_bet_player;
        asset down_max_bet;

        
        for (auto tt = itr_lower; tt != itr.end(); tt++)
        {
            if (tt->max == 1)
            {
                down += tt->bet.amount;
                up_all[tt->owner] += tt->bet.amount;
                if (up_max_bet.amount < tt->bet.amount)
                {
                    up_max_player = tt->owner;
                    up_max_bet = tt->bet;
                }
            }
            else
            {
                up += tt->bet.amount;
                down_all[tt->owner] += tt->bet.amount;
                if (down_max_bet.amount < tt->bet.amount)
                {
                    down_max_bet = tt->bet;
                    down_max_bet_player = tt->owner;
                }
            }
        }
        print("up size = ", up_all.size(), "  down size = ", down_all.size(), "\n");

        eosio_assert((up != 0 && down !=0), "not enough offer.");
        // uint32_t percent = 10000;
        if (rand == 1)
        { //
            uint32_t cost = 0;
            for (auto &up_itr : up_all)
            {
                auto to_player = accounts.find(up_itr.first);
                uint32_t to_amount = down * up_itr.second / up;
                cost += to_amount;
                accounts.modify(to_player, 0, [&](auto &obj){
                    obj.balance.amount += to_amount;
                });
            }
            eosio_assert(cost <= down , "error cost < down!!!!!\n");
            if(cost < down){
                auto to_player = accounts.find(up_max_player);
                accounts.modify(to_player, 0, [&](auto &obj){
                    obj.balance.amount += down - cost;
                });
            }
            for(auto &down_itr : down_all){
                auto from_player = accounts.find(down_itr.first);
                accounts.modify(from_player, 0, [&](auto &obj){
                    obj.balance.amount -= down_itr.second;
                });
            }

        }else{
            uint32_t cost = 0;
            for (auto &down_itr : down_all)
            {
                auto to_player = accounts.find(down_itr.first);
                uint32_t to_amount = up * down_itr.second / down;
                cost += to_amount;
                accounts.modify(to_player, 0, [&](auto &obj){
                    obj.balance.amount += to_amount;
                });
            }
            eosio_assert(cost <= up , "error cost < up!!!!!\n");
            if(cost < up){
                auto to_player = accounts.find(down_max_bet_player);
                accounts.modify(to_player, 0, [&](auto &obj){
                    obj.balance.amount += down - cost;
                });
            }
            for(auto &down_itr : up_all){
                auto from_player = accounts.find(down_itr.first);
                accounts.modify(from_player, 0, [&](auto &obj){
                    obj.balance.amount -= down_itr.second;
                });
            }
        }
    }

    /// @abi action
    void deposit(const account_name from, const asset &bet)
    {
        eosio_assert(bet.is_valid(), "bet is invalid\n");
        eosio_assert(bet.amount > 0, "bet.amount must > 0.\n");
        auto itr = accounts.find(from);
        if (itr == accounts.end())
        {
            itr = accounts.emplace(_self, [from](auto &obj) {
                obj.owner = from;
            });
        }

        action(
            permission_level{from, N(active)},
            N(test.token), N(transfer),
            std::make_tuple(from, _self, bet, std::string("")))
            .send();

        accounts.modify(itr, 0, [&](auto &obj) {
            obj.balance.amount += bet.amount;
            obj.balance.symbol = bet.symbol;
        });
    }

  private:
    // /*
    //     game object
    // */
    // struct game
    // {
    //     uint64_t id;
    //     asset maxbet;
    //     asset minbet;
    //     eosio::time_point_sec deadline;

    //     EOSLIB_SERIALIZE(game, (id)(maxbet)(minbet)(deadline))
    // };

    // typedef eosio::multi_index<N(game), game> game_index;

    /*
        accounts
    */
    struct account
    {
        account(account_name o = account_name()) : owner(o)
        {
        }
        account_name owner;
        asset balance;
        uint32_t opengames;
        uint64_t primary_key() const { return owner; }

        EOSLIB_SERIALIZE(account, (owner)(balance)(opengames))
    };

    typedef eosio::multi_index<N(account), account> account_index;

    struct globalgame
    {
        uint64_t id;
        uint64_t nextgameid = 0;
        uint32_t islocked  = 0;
        uint32_t lockblocknum = 0;
        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(globalgame, (id)(nextgameid)(islocked)(lockblocknum))
    };

    typedef eosio::multi_index<N(global), globalgame> globalindex;

    struct offer
    {
        uint64_t id;
        account_name owner;
        asset bet;
        uint32_t max; //true max, false min
        uint64_t gameid = 0;
        account_name by_owner() const { return owner; }
        uint64_t by_game_id() const { return gameid; }
        uint64_t by_bet() const { return (uint64_t)bet.amount; }
        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(offer, (id)(owner)(bet)(max)(gameid))
    };

    typedef eosio::multi_index<N(offer), offer,
                               indexed_by<N(owner), const_mem_fun<offer, account_name, &offer::by_owner>>,
                               indexed_by<N(gameid), const_mem_fun<offer, uint64_t, &offer::by_game_id>>>
        offer_index;

    // /*
    //         member
    // */
    // game_index game;
    globalindex globalgame;
    account_index accounts;
    offer_index offers;
};

// EOSIO_ABI(playcoin, (pushbet)(cancelbet)(lockgame)(openresult))
EOSIO_ABI(playcoin, (pushbet)(cancelbet)(lockgame)(openresult)(deposit))
