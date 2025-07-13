#include "history.h"
#include <cstring>

History::History(const Player& local) : _local(local) {
    memset(&_transactions,0,sizeof(_transactions));
    memset(&_games,0,sizeof(_games));
};

bool History::addTransactions(const Transaction& tran1, const Transaction tran2, const GameState& prevGame) {
    //would moving forward by one discard a desync?
    if(_transactions[Player::ONE][0].noOp || _transactions[Player::TWO][0].noOp)
        return false;
    
    // shift back one
    auto shiftIter = _transactions[Player::ONE].begin();
    auto shiftIter2 = shiftIter;
    ++shiftIter2;
    while(shiftIter2 != _transactions[Player::ONE].end()) {
        *shiftIter = *shiftIter2;
        ++shiftIter;
        ++shiftIter2;
    };
    
    shiftIter = _transactions[Player::TWO].begin();
    shiftIter2 = shiftIter;
    ++shiftIter2;
    while(shiftIter2 != _transactions[Player::TWO].end()) {
        *shiftIter = *shiftIter2;
        ++shiftIter;
        ++shiftIter2;
    };
    
    auto gshiftIter = _games.begin();
    auto gshiftIter2 = gshiftIter;
    ++gshiftIter2;
    while(gshiftIter2 != _games.end()) {
        *gshiftIter = *gshiftIter2;
        ++gshiftIter;
        ++gshiftIter2;
    };
    
    // update noOpIndex
    for(auto iter = _transactions[Player::ONE].begin(); iter != _transactions[Player::ONE].end(); ++iter) {
        if(iter->noOp && iter->nextNoOpIndex != END_INDEX)
            iter->nextNoOpIndex--;
    }
    for(auto iter = _transactions[Player::TWO].begin(); iter != _transactions[Player::TWO].end(); ++iter) {
        if(iter->noOp && iter->nextNoOpIndex != END_INDEX)
            iter->nextNoOpIndex--;
    }

    // add last
    _transactions[Player::ONE][TRANSACTION_COUNT - 1] = tran1;
    _transactions[Player::ONE][TRANSACTION_COUNT - 1].nextNoOpIndex = END_INDEX;
    _transactions[Player::TWO][TRANSACTION_COUNT - 1] = tran2;
    _transactions[Player::TWO][TRANSACTION_COUNT - 1].nextNoOpIndex = END_INDEX;

    // update last NoOp
    if(tran1.noOp) {
        auto iter = _transactions[Player::ONE].rbegin();
        iter++;
        while(iter != _transactions[Player::ONE].rend()) {
            if(iter->noOp) {
                iter->nextNoOpIndex = TRANSACTION_COUNT - 1;
                break;
            }
            iter++;
        }
    }
    if(tran2.noOp) {
        auto iter = _transactions[Player::TWO].rbegin();
        iter++;
        while(iter != _transactions[Player::TWO].rend()) {
            if(iter->noOp) {
                iter->nextNoOpIndex = TRANSACTION_COUNT - 1;
                break;
            }
            iter++;
        }
    }

    return true;
};

uint32_t History::amendTransactions(const Transaction& tran, const Player& p) {
    for(int i = 0; i < TRANSACTION_COUNT; i++) {
        if(tran.frame == _transactions[p][i].frame) {
            for(int j = i - 1; j >= 0; j--) {
                if(_transactions[p][j].noOp) {
                    _transactions[p][j].nextNoOpIndex = _transactions[p][i].nextNoOpIndex;
                    break;
                }
            }
            _transactions[p][i] = tran;
            return i;
        }
    }

    //was not able to amend
    return TRANSACTION_COUNT;
};

const History::Transaction History::getNoOps(const Player& p) {
    for(auto iter = _transactions[p].cbegin(); iter != _transactions[p].cend(); ++iter) {
        if(iter->noOp)
            return *iter;
    }
    Transaction t;
    t.frame--;
    return t;
};

History::Transaction History::transactionAt(const uint32_t& index, const Player& p) {
    return _transactions[p][index];
};

History::GameState History::gameAt(const uint32_t& index) {
    return _games[index];
};