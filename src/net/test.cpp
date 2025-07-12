#include "history.h"
#include <iostream>

int main(int argc, char** argv) {
    // fill in test data
    std::array<History::InputsHistory,15> player1Inputs;
    std::array<History::InputsHistory,15> player2Inputs;

    std::array<History::Input,15> player1Turns;
    std::array<History::Input,15> player2Turns;

    for(int i = 0; i < player1Turns.size(); i++) {
        player1Turns[i][0] = i+1;
    }
    for(int i = 0; i < player2Turns.size(); i++) {
        player2Turns[i][0] = i+20;
    }

    // TODO: initialize input histories
    

    //try out some cases
    
    ////////////////// 1 input from each
    

    //////////////// all inputs from each
    


    

    std::cout << "Done!" << std::endl;

};