#ifndef HISTORY_H
#define HISTORY_h

#include <cstdint>
#include <array>
#include "net_types.h"

class History {
    public:
        //typedefs and consts
        static const uint32_t TRANSACTION_COUNT = 10;
        static const uint32_t PLAYER_COUNT = 2;
        static const uint32_t END_INDEX = 0 - 1;
        enum Player : uint32_t  {
            ONE = 0,
            TWO = 1,
            SPECTATE = 2
        };

        struct Transaction {
            NetTypes::Input in;
            uint32_t frame;
            bool noOp;

            // only relevant if noOp is true
            // only live until next addTransactions
            // expected to be END_INDEX if noOp = false
            uint32_t nextNoOpIndex;
        };

        struct GameState {
            NetTypes::Game game;
            uint32_t frame;
        };

        typedef std::array<Transaction, TRANSACTION_COUNT> TransactionHistory;
        typedef std::array<TransactionHistory, PLAYER_COUNT> PlayerTransactions;
        typedef std::array<GameState,TRANSACTION_COUNT> GameHistory;
        


        // public functions

        // run once per frame
        // will forward transactions by one
        // tran1 = player 1's
        // tran2 = player 2's
        // remember which player is 1 or 2 should be consistent across machines!
        // will not forward if desynced. always check!
        // returns false if desynced, true if synced 
        bool addTransactions(const Transaction& tran1, const Transaction tran2, const GameState& prevGame);

        // will not forward transactions
        // returns index of start of reprocess
        // assumes this transaction is NOT a noOp
        uint32_t amendTransactions(const Transaction& tran, const Player& p);

        // returns farthest back noOp
        // check transaction for next noOp
        // returns Transaction with frame max(uint32_t) if no noOps;
        const Transaction getNoOps(const Player& p);

        Transaction transactionAt(const uint32_t& index, const Player& p);
        GameState& gameAt(const uint32_t& index);

        History(const Player& local);
    private:

        // helper functions

        // member vars
        Player _local;
        // Game history trails one behind Transactions
        // e.g. current game[1] = game[0] + inputs[0]

        // rewrites on append (optimal?)
        GameHistory _games;

        // rewrites on append (optimal?)
        PlayerTransactions _transactions;
};

#endif //HISTORY_H