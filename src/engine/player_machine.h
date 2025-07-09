#ifndef PLAYER_MACHINE_H
#define PLAYER_MACHINE_H

#include <functional>
#include "state_machine.h"
#include "input_machine.h"
#include "move_machine.h"
#include "block_machine.h"
#include "attack_machine.h"
#include <string>

namespace PlayerMachine {
    struct In {
        InputMachine::Out keys;
    };
    struct Out {
        uint8_t px, py;
        std::string sprite;
        uint8_t hitStartX, hitEndX, hitY;
        bool blocking;
    };

    class Machine : public StateMachine<In,Out> {
        private:
            // vars
            uint8_t _px, _py;

            // inner state machines
            MoveMachine::Machine _moveMachine;
            BlockMachine::Machine _blockMachine;
            AttackMachine::Machine _attackMachine;
            Out _ret;

            // state functions
            static const Out& _universal(StateMachine<In,Out>* sm, const In& in) {
                Machine* m = static_cast<Machine*>(sm);
                MoveMachine::In moveIn;
                moveIn.keys = in.keys;
                moveIn.px = m->_px;
                moveIn.py = m->_py;
                MoveMachine::Out moveOut = m->_moveMachine.step(moveIn);
                m->_px = moveOut.px;
                m->_py = moveOut.py;
                BlockMachine::In blockIn;
                blockIn.keys = in.keys;
                blockIn.movingBack = moveOut.movingBack;
                BlockMachine::Out blockOut = m->_blockMachine.step(blockIn);
                
                
                m->_ret.blocking = false;
                m->_ret.hitEndX = 0;
                m->_ret.hitStartX = 0;
                m->_ret.hitY = 0;
                if(moveOut.inMove) {
                    if(!moveOut.movingBack) {
                        AttackMachine::In attackIn;
                        attackIn.keys = in.keys;
                        AttackMachine::Out attackOut = m->_attackMachine.step(attackIn);
                        if(!strcmp(attackOut.sprite.c_str(),"Idle")) {
                            m->_ret.sprite = attackOut.sprite;
                            m->_ret.hitStartX = attackOut.hitStartX;
                            m->_ret.hitEndX = attackOut.hitEndX;
                            m->_ret.hitY = attackOut.hitY;
                        } else {
                            m->_ret.sprite = "Move";
                        }
                    } else {
                        m->_ret.sprite = "Move";
                        m->_ret.blocking = true;
                    }
                } else if(blockOut.blocking) {
                    m->_ret.sprite = "Block";
                    m->_ret.blocking = true;
                } else {
                    AttackMachine::In attackIn;
                    attackIn.keys = in.keys;
                    AttackMachine::Out attackOut = m->_attackMachine.step(attackIn);
                    m->_ret.sprite = attackOut.sprite;
                    m->_ret.hitStartX = attackOut.hitStartX;
                    m->_ret.hitEndX = attackOut.hitEndX;
                    m->_ret.hitY = attackOut.hitY;
                }
                m->_ret.px = m->_px;
                m->_ret.py = m->_py;
                return m->_ret;
            };

            // state enum
            enum State : uint8_t {
                UNIVERSAL
            };

        public:
            Machine() {
                _px = (MoveMachine::Machine::TILE_WIDTH / 2) * 3;
                _py = (MoveMachine::Machine::TILE_HEIGHT / 2) * 3;
                _changeState(this, _universal,UNIVERSAL);
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
                size += sizeof(frame);
                _changeFrameOnly(frame);

                _px = *reinterpret_cast<uint8_t*>(addr+size);
                size += sizeof(_px);

                _py = *reinterpret_cast<uint8_t*>(addr+size);
                size += sizeof(_py);

                _moveMachine = MoveMachine::Machine(addr,size);
                _blockMachine = BlockMachine::Machine(addr,size);
                _attackMachine = AttackMachine::Machine(addr,size);
            };
            size_t serialize(char* addr) {
                size_t offset = 0;
                State sEnum = (State) _currentEnum();
                offset += serializeBytes(addr+offset, sEnum);
                offset += serializeBytes(addr+offset,_currentFrame());
                offset += serializeBytes(addr+offset,_px);
                offset += serializeBytes(addr+offset,_py);
                offset += _moveMachine.serialize(addr+offset);
                offset += _blockMachine.serialize(addr+offset);
                offset += _attackMachine.serialize(addr+offset);

                return offset;
            };
    };
};

#endif //PLAYER_MACHINE_H