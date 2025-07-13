#include <iostream>
#include "net_types.h"
#include "history.h"
#include <array>
#include <cstring>


int main(int argc, char** argv) {
    
    // initialize test data
    const uint32_t testSize = History::TRANSACTION_COUNT + 10;
    std::array<History::Transaction,testSize> player1Trans;
    std::array<History::Transaction,testSize> player2Trans;

    for(int i = 0; i < testSize; i++) {
        player1Trans[i].frame = i;
        player1Trans[i].noOp = false;
        player1Trans[i].nextNoOpIndex = History::END_INDEX;
        memset(&player1Trans[i].in,0,sizeof(player1Trans[i].in));
        player1Trans[i].in[0] = i+1;

        player2Trans[i].frame = i;
        player2Trans[i].noOp = false;
        player2Trans[i].nextNoOpIndex = History::END_INDEX;
        memset(&player2Trans[i].in,0,sizeof(player2Trans[i].in));
        player2Trans[i].in[0] = i+101;
    }

    History::GameState game;
    memset(&game,0,sizeof(game));

    History h(History::Player::SPECTATE);

    ////////////////////////////////////////////////////
    std::cout << "----------fill transaction history" << std::endl;
    for(int i = 0; i < History::TRANSACTION_COUNT; i++) {
        h.addTransactions(player1Trans[i],player2Trans[i],game);
    };
    for(int i = 0; i < History::TRANSACTION_COUNT; i++) {
        History::Transaction t = h.transactionAt(i,History::Player::ONE);
        if(memcmp(&t,&player1Trans[i],sizeof(t)))
            std::cout << "mismatch of transactions!" << std::endl;
        
        t = h.transactionAt(i,History::Player::TWO);
        if(memcmp(&t,&player2Trans[i],sizeof(t)))
            std::cout << "mismatch of transactions!" << std::endl;
    };
    std::cout << "done" << std::endl;
    
    ////////////////////////////////////////////////////
    std::cout << "----------fill transaction history + more" << std::endl;
    h = History(History::Player::SPECTATE);
    for(int i = 0; i < player1Trans.size(); i++) {
        h.addTransactions(player1Trans[i],player2Trans[i],game);
    };
    for(int i = 0; i < History::TRANSACTION_COUNT; i++) {
        History::Transaction t = h.transactionAt(i,History::Player::ONE);
        if(memcmp(&t,&player1Trans[i+10],sizeof(t)))
            std::cout << "mismatch of transactions!" << std::endl;
        
        t = h.transactionAt(i,History::Player::TWO);
        if(memcmp(&t,&player2Trans[i+10],sizeof(t)))
            std::cout << "mismatch of transactions!" << std::endl;
    };
    std::cout << "done" << std::endl;

    ////////////////////////////////////////////////////
    std::cout << "----------single desync" << std::endl;
    h = History(History::Player::SPECTATE);
    player1Trans[0].noOp = true;
    for(int i = 0; i < History::TRANSACTION_COUNT; i++) {
        if(!h.addTransactions(player1Trans[i],player2Trans[i],game))
            std::cout << "error before desync" << std::endl;
    }
    if(h.addTransactions(player1Trans[History::TRANSACTION_COUNT],player2Trans[History::TRANSACTION_COUNT],game))
        std::cout << "desync didn't happen!" << std::endl;
    std::cout << "done" << std::endl;

    ////////////////////////////////////////////////////
    std::cout << "----------amend a transaction" << std::endl;
    h = History(History::Player::SPECTATE);
    player1Trans[0].noOp = true;
    for(int i = 0; i < History::TRANSACTION_COUNT; i++) {
        if(!h.addTransactions(player1Trans[i],player2Trans[i],game))
            std::cout << "error before desync" << std::endl;
    }
    player1Trans[0].noOp = false;
    uint32_t indexToReprocess = h.amendTransactions(player1Trans[0],History::Player::ONE);
    if(indexToReprocess != 0)
        std::cout << "wrong index received!" << std::endl;
    History::Transaction t = h.transactionAt(indexToReprocess,History::Player::ONE);
    if(memcmp(&t,&player1Trans[0],sizeof(t)))
        std::cout << "not amended!" << std::endl;
    std::cout << "done" << std::endl;
    
    ////////////////////////////////////////////////////
    std::cout << "----------chaining noop" << std::endl;
    h = History(History::Player::SPECTATE);
    player1Trans[0].noOp = true;
    player1Trans[3].noOp = true;
    player1Trans[5].noOp = true;
    for(int i = 0; i < History::TRANSACTION_COUNT; i++) {
        if(!h.addTransactions(player1Trans[i],player2Trans[i],game))
            std::cout << "error before desync" << std::endl;
    }
    
    t = h.getNoOps(History::Player::ONE);
    if(t.frame != 0)
        std::cout << "Frame 0 error" << std::endl;
        
    t = h.transactionAt(t.nextNoOpIndex,History::Player::ONE);
    if(t.frame != 3)
        std::cout << "Frame 3 error" << std::endl;
    
    t = h.transactionAt(t.nextNoOpIndex,History::Player::ONE);
    if(t.frame != 5)
        std::cout << "Frame 5 error" << std::endl;
    
    if(t.nextNoOpIndex != History::END_INDEX) 
        std::cout << "-1 terminator error";
    
    std::cout << "done" << std::endl;
    
};