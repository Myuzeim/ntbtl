#ifndef HISTORY_H
#define HISTORY_H

#include <cstddef>
#include <array>
#include <cstdint>
#include <cstring>

class History {
    private:
        ////////// Consts

        // This should be big enough for a serialized game state
        static const size_t GAME_SIZE = 512;

        // This should be big enough for a serialized frame input
        static const size_t INPUT_SIZE = 64;
        
        // How many frames back history goes
        // Keep < 1/2 of size_t
        static const size_t TRANSACTION_COUNT = 10;

    public:
        // buffers
        typedef std::array<uint8_t, GAME_SIZE> Game;
        typedef std::array<uint8_t, INPUT_SIZE> Input;
        typedef std::array<Input,TRANSACTION_COUNT> InputsHistory;
        typedef std::array<Game,TRANSACTION_COUNT> GameHistory;


        struct Transaction {
            // frame count
            // 1 game = 99 sec * 30 fps ~= 3000 frames to process
            size_t frame;

            // did we receive an input for this frame?
            bool noOp;

            // Irrelevant if NoOp.
            // Remember to 0 out
            Input in;
        };

        typedef std::array<Transaction,TRANSACTION_COUNT> TransactionHistory;

        enum Player : size_t {
            ONE = 0,
            TWO = 1
        };

    private:
        //////////// Member vars

        // last in array is the most recent
        GameHistory _games;

        // primary data structure
        // last in array is most recent
        std::array<TransactionHistory,2> _transactions;

        // bucket helpers
        GameHistory _gameBucket;
        TransactionHistory _transactionBucket;

        // Least recent newly filled in transaction
        std::array<size_t,2> _askIndex;

        // helper for inputBoth
        void inputPlayer(const InputsHistory& inputs, const bool& noOp, const Player& p);
    public:
        History();

        // Ideally you call for inputs for each player
        // then you place output into the GameHistory
        TransactionHistory::const_iterator askedInputsProcess(const Player& p);
        GameHistory::iterator askedToProcess();

        // expected every frame, even if there's no input
        // (in that case noOp = true)
        void inputBoth(const InputsHistory& inputs1, const bool& noOp1, const InputsHistory& inputs2, const bool& noOp2);
        
};

#endif  //HISTORY_H