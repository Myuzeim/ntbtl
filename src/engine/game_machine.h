#ifndef GAME_MACHINE_H
#define GAME_MACHINE_H

#include "state_machine.h"
#include "player_machine.h"
#include <functional>

namespace GameMachine {
    struct In {
        InputMachine::In player1Keys;
        InputMachine::In player2Keys;
    };
    struct Out {
        PlayerMachine::Out player1Data;
        PlayerMachine::Out player2Data;
        uint8_t player1hp;
        uint8_t player2hp;
    };

    class Machine : public StateMachine<In,Out> {
        private:

            // inner state machines
            PlayerMachine::Machine _player1;
            PlayerMachine::Machine _player2;
            InputMachine::Machine _player1Input;
            InputMachine::Machine _player2Input;

            // vars
            uint8_t _player1hp;
            uint8_t _player2hp;

            // state functions
            static Out _universal(StateMachine<In,Out>* sm, const In& in) {
                Machine* m = static_cast<Machine*>(sm);
                Out ret;
                InputMachine::Out player1Input = m->_player1Input.step(in.player1Keys);
                InputMachine::Out player2Input = m->_player2Input.step(in.player2Keys);
                PlayerMachine::In player1In, player2In;
                player1In.keys = player1Input;
                player2In.keys = player2Input;
                ret.player1Data = m->_player1.step(player1In);
                ret.player2Data = m->_player2.step(player2In);

                //hit scan
                // think this doesnt reflect right?
                if(ret.player1Data.hitY + ret.player1Data.py >= ret.player2Data.py + MoveMachine::Machine::TILE_HEIGHT && ret.player1Data.hitY + ret.player1Data.py <= ret.player2Data.py + 2 * MoveMachine::Machine::TILE_HEIGHT)
                {
                    uint8_t real2Px = 4 * MoveMachine::Machine::TILE_WIDTH - ret.player2Data.px;
                    if(real2Px > ret.player1Data.px + ret.player1Data.hitStartX && real2Px < ret.player1Data.px + ret.player1Data.hitEndX)
                        m->_player2hp -= 10;
                }
                if(ret.player2Data.hitY + ret.player2Data.py >= ret.player1Data.py + MoveMachine::Machine::TILE_HEIGHT && ret.player2Data.hitY + ret.player2Data.py <= ret.player1Data.py + 2 * MoveMachine::Machine::TILE_HEIGHT)
                {
                    uint8_t real2PxStart = 4 * MoveMachine::Machine::TILE_WIDTH - std::min(240,ret.player2Data.px + ret.player2Data.hitEndX);
                    uint8_t real2PxEnd = 4 * MoveMachine::Machine::TILE_WIDTH - std::min(240,ret.player2Data.px + ret.player2Data.hitStartX);
                    if(ret.player1Data.px > real2PxStart && ret.player1Data.px < real2PxEnd)
                        m->_player1hp -= 10;
                }
                ret.player1hp = m->_player1hp;
                ret.player2hp = m->_player2hp;

                return ret;
            };
            
        public:
            Machine() {
                _player1hp = 100;
                _player2hp = 100;
                _changeState(this, _universal);
            };
            
    };
};

#endif //GAME_MACHINE_H