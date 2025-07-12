#include "history.h"
#include <iostream>

History::History() {
    //initialize all to 0
    memset(&_transactions[0],0,sizeof(_transactions));
    for(auto iter = _transactions[0].begin(); iter != _transactions[0].end(); iter++) {
        iter->noOp = true;
    };
    for(auto iter = _transactions[1].begin(); iter != _transactions[1].end(); iter++) {
        iter->noOp = true;
    };
};

void History::inputPlayer(const InputsHistory& inputs, const bool& noOp, const Player& p) {
    
};

void History::inputBoth(const InputsHistory& inputs1, const bool& noOp1, const InputsHistory& inputs2, const bool& noOp2) {

};

History::TransactionHistory::const_iterator History::askedInputsProcess(const Player& p) {

};

History::GameHistory::iterator History::askedToProcess() {

};