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

            // state functions
            static Out _universal(StateMachine<In,Out>* sm, const In& in) {
                Machine* m = static_cast<Machine*>(sm);
                Out ret;
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
                
                
                ret.blocking = false;
                ret.hitEndX = 0;
                ret.hitStartX = 0;
                ret.hitY = 0;
                if(moveOut.inMove) {
                    if(!moveOut.movingBack) {
                        AttackMachine::In attackIn;
                        attackIn.keys = in.keys;
                        AttackMachine::Out attackOut = m->_attackMachine.step(attackIn);
                        if(!strcmp(attackOut.sprite.c_str(),"Idle")) {
                            ret.sprite = attackOut.sprite;
                            ret.hitStartX = attackOut.hitStartX;
                            ret.hitEndX = attackOut.hitEndX;
                            ret.hitY = attackOut.hitY;
                        } else {
                            ret.sprite = "Move";
                        }
                    } else {
                        ret.sprite = "Move";
                        ret.blocking = true;
                    }
                } else if(blockOut.blocking) {
                    ret.sprite = "Block";
                    ret.blocking = true;
                } else {
                    AttackMachine::In attackIn;
                    attackIn.keys = in.keys;
                    AttackMachine::Out attackOut = m->_attackMachine.step(attackIn);
                    ret.sprite = attackOut.sprite;
                    ret.hitStartX = attackOut.hitStartX;
                    ret.hitEndX = attackOut.hitEndX;
                    ret.hitY = attackOut.hitY;
                }
                ret.px = m->_px;
                ret.py = m->_py;
                return ret;
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
                memcpy(addr,&sEnum,sizeof(sEnum));
                offset+=sizeof(sEnum);

                uint8_t frame = _currentFrame();
                memcpy(addr+offset,&frame,sizeof(frame));
                offset+=sizeof(frame); 

                memcpy(addr+offset,&_px,sizeof(_px));
                offset+=sizeof(_px);

                memcpy(addr+offset,&_py,sizeof(_py));
                offset+=sizeof(_py);

                offset += _moveMachine.serialize(addr+offset);
                offset += _blockMachine.serialize(addr+offset);
                offset += _attackMachine.serialize(addr+offset);

                return offset;
            };
    };
};

#endif //PLAYER_MACHINE_H