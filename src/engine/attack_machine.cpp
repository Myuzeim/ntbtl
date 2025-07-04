#include "attack_machine.h"
#include "move_machine.h"
#include <cstring>

namespace AttackMachine {
    Machine::Machine() {
        _punching = false;
        _changeState(this,_idle,IDLE);
    };

    const Out Machine::_idle(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        m->_punching = false;
        if(in.keys.keys.Attack1) {
            m->_punching = true;
            m->_changeState(m,_windup,WINDUP);
        } else if(in.keys.keys.Attack2) {
            m->_punching = false;
            m->_changeState(m,_windup,WINDUP);
        }
        return {"Idle",0,0,0};
    };

    const Out Machine::_windup(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        if(m->_punching) {
            if(m->_currentFrame() >= Machine::punchWindupSpeed) {
                m->_changeState(m,_follow,FOLLOW);
                return {"PunchFollow",0,MoveMachine::Machine::TILE_WIDTH * 2,MoveMachine::Machine::TILE_HEIGHT * 3 / 2};
            }
            return {"PunchWindup",0,0,0};
        } else {
            if(m->_currentFrame() >= Machine::kickWindupSpeed) {
                m->_changeState(m,_follow,FOLLOW);
                return {"KickFollow",0,MoveMachine::Machine::TILE_WIDTH * 3,MoveMachine::Machine::TILE_HEIGHT * 3 / 2};
            }
            return {"KickWindup",0,0,0};
        }
    };

    const Out Machine::_follow(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        if(m->_punching) {
            if(m->_currentFrame() >= Machine::punchFollowSpeed) {
                m->_punching = false;
                m->_changeState(m,_idle,FOLLOW);
                return {"Idle",0,0,0};
            }
            return {"PunchFollow",0,0};
        } else {
            if(m->_currentFrame() >= Machine::kickFollowSpeed) {
                m->_punching = false;
                m->_changeState(m,_idle,FOLLOW);
                return {"Idle",0,0,0};
            }
            return {"KickFollow",0,0};
        }
    };

    size_t Machine::serialize(char* addr) {    
        size_t offset = 0;
        State sEnum = (State) _currentEnum();
        memcpy(addr,&sEnum,sizeof(sEnum));
        offset+=sizeof(sEnum);

        uint8_t frame = _currentFrame();
        memcpy(addr+offset,&frame,sizeof(frame));
        offset+=sizeof(frame); 

        memcpy(addr+offset,&_punching,sizeof(_punching));
        offset+=sizeof(_punching);

        return offset;
    };

    //deserialize
    //size param is modified to increment the size of the object created
    Machine::Machine(char* addr, size_t& size) {
        State sEnum = *reinterpret_cast<State*>(addr+size);
        size += sizeof(sEnum);
        switch(sEnum) {
            case IDLE:
                _changeState(this,_idle,IDLE);
                break;
            case WINDUP:
                _changeState(this,_windup,WINDUP);
                break;
            case FOLLOW:
                _changeState(this,_follow,FOLLOW);
                break;
            default:
                break; //error!
        }

        uint8_t frame = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(sEnum);
        _changeFrameOnly(frame);

        _punching = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_punching);
    };
};