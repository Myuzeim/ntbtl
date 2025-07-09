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
        void clear() {
            player1Data = {};
            player2Data = {};
            player1hp = 0;
            player2hp = 0;
        };
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
            Out _ret;

            // state functions
            static const Out& _universal(StateMachine<In,Out>* sm, const In& in) {
                Machine* m = static_cast<Machine*>(sm);
                m->_ret.clear();

                InputMachine::Out player1Input = m->_player1Input.step(in.player1Keys);
                InputMachine::Out player2Input = m->_player2Input.step(in.player2Keys);
                PlayerMachine::In player1In, player2In;
                player1In.keys = player1Input;
                player2In.keys = player2Input;
                m->_ret.player1Data = m->_player1.step(player1In);
                m->_ret.player2Data = m->_player2.step(player2In);

                //hit scan
                //TODO: refactor!
                if(m->_ret.player1Data.hitY + m->_ret.player1Data.py >= m->_ret.player2Data.py + MoveMachine::Machine::TILE_HEIGHT && m->_ret.player1Data.hitY + m->_ret.player1Data.py <= m->_ret.player2Data.py + 2 * MoveMachine::Machine::TILE_HEIGHT)
                {
                    uint8_t real2Px = 4 * MoveMachine::Machine::TILE_WIDTH - m->_ret.player2Data.px;
                    if(real2Px > m->_ret.player1Data.px + m->_ret.player1Data.hitStartX && real2Px < m->_ret.player1Data.px + m->_ret.player1Data.hitEndX)
                        if(!m->_ret.player2Data.blocking)
                            if(!(m->_ret.player1Data.hitEndX == 0 && m->_ret.player1Data.hitStartX && m->_ret.player1Data.hitY)) //is there even an attack? 
                                m->_player2hp -= 10;
                }
                if(m->_ret.player2Data.hitY + m->_ret.player2Data.py >= m->_ret.player1Data.py + MoveMachine::Machine::TILE_HEIGHT && m->_ret.player2Data.hitY + m->_ret.player2Data.py <= m->_ret.player1Data.py + 2 * MoveMachine::Machine::TILE_HEIGHT)
                {
                    uint8_t real2PxStart = 4 * MoveMachine::Machine::TILE_WIDTH - std::min(240,m->_ret.player2Data.px + m->_ret.player2Data.hitEndX);
                    uint8_t real2PxEnd = 4 * MoveMachine::Machine::TILE_WIDTH - std::min(240,m->_ret.player2Data.px + m->_ret.player2Data.hitStartX);
                    if(m->_ret.player1Data.px > real2PxStart && m->_ret.player1Data.px < real2PxEnd)
                        if(!m->_ret.player1Data.blocking)
                            if(!(m->_ret.player2Data.hitEndX == 0 && m->_ret.player2Data.hitStartX && m->_ret.player2Data.hitY)) //is there even an attack?
                                m->_player1hp -= 10;
                }
                m->_ret.player1hp = m->_player1hp;
                m->_ret.player2hp = m->_player2hp;

                return m->_ret;
            };

            // State Enum
            enum State : uint8_t {
                UNIVERSAL
            };
            
        public:
            Machine() {
                _player1hp = 100;
                _player2hp = 100;
                _changeState(this, _universal,UNIVERSAL);
            };
            
            size_t serialize(char* addr) {
                size_t offset = 0;
                State sEnum = (State) _currentEnum();
                offset += serializeBytes(addr+offset, sEnum);
                offset += serializeBytes(addr+offset,_currentFrame());
                offset += serializeBytes(addr+offset,_player1hp);
                offset += serializeBytes(addr+offset,_player2hp);
                offset += _player1.serialize(addr+offset);
                offset += _player2.serialize(addr+offset);
                offset += _player1Input.serialize(addr+offset);
                offset += _player2Input.serialize(addr+offset);

                return offset;
            };

            //deserialize
            //size param is modified to increment the size of the object created
            Machine(char* addr, size_t& size) {
                State sEnum = *reinterpret_cast<State*>(addr+size);
                size += sizeof(sEnum);
                switch(sEnum) {
                    case UNIVERSAL:
                        _changeState(this,_universal,UNIVERSAL);
                        break;
                    default:
                        break; //error!
                };

                uint8_t frame = *reinterpret_cast<uint8_t*>(addr+size);
                size += sizeof(sEnum);
                _changeFrameOnly(frame);

                _player1hp = *reinterpret_cast<uint8_t*>(addr+size);
                size += sizeof(_player1hp);

                _player2hp = *reinterpret_cast<uint8_t*>(addr+size);
                size += sizeof(_player2hp);

                _player1 = PlayerMachine::Machine(addr,size);
                _player2 = PlayerMachine::Machine(addr,size);
                _player1Input = InputMachine::Machine(addr,size);
                _player2Input = InputMachine::Machine(addr,size);
            };
    };
};

#endif //GAME_MACHINE_H