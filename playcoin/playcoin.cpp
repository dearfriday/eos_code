#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>

using eosio::key256;
using eosio::indexed_by;
using eosio::const_mem_fun;
using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;

class playcoin : public eosio::contract
{
  public:
    playcoin(account_name self)
        : eosio::contract(self)
          // , game(_self, _self),
          //   globalgame(_self, _self),
          ,
          accounts(_self, _self)
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
        // auto gl_game = globalgame.begin();
        // if (gl_game == globalgame.end())
        // {
        //     gl_game = globalgame.emplace(_self, [](auto &gl) {
        //         gl.nextGameId = 0;
        //     });
        // }
        // else
        // {
        // }
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
    }

    // /// @abi action
    void openresult(const account_name &player)
    {
        eosio_assert(player == _self, "only self can open result");
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
            permission_level{ from, N(active) },
            N(test.token), N(transfer),
            std::make_tuple(from, _self, bet, std::string(""))
         ).send();



        accounts.modify(itr, 0, [&](auto &obj){
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

    // struct globalgame
    // {
    //     uint64_t id;
    //     uint64_t nextgameid = 0;
    //     bool islocked = false;
    //     uint64_t primary_key() const { return id; }

    //     EOSLIB_SERIALIZE(globalgame, (id)(nextgameid)(islocked))
    // };

    // typedef eosio::multi_index<N(global), globalgame> globalindex;

    // struct offer
    // {
    //     uint64_t id;
    //     account_name owner;
    //     asset bet;
    //     bool max; //true max, false min
    //     checksum256 commitment;
    //     uint64_t gameid = 0;

    //     uint64_t by_bet() const { return (uint64_t)bet.amount; }
    //     EOSLIB_SERIALIZE(offer, (id)(owner)(commitment)(gameid))
    // };

    // typedef eosio::multi_index<N(offer), offer> offer_index;

    // /*
    //         member
    // */
    // game_index game;
    // globalindex globalgame;
    account_index accounts;
};

// EOSIO_ABI(playcoin, (pushbet)(cancelbet)(lockgame)(openresult))
EOSIO_ABI(playcoin, (pushbet)(cancelbet)(lockgame)(openresult)(deposit))
