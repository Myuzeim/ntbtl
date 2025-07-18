#include "attack_machine.h"
#include "move_machine.h"
#include <cstring>

namespace AttackMachine {
    Machine::Machine() {
        _punching = false;
        _initStateMap();
        _changeState(this,IDLE);
    };

    const Out& Machine::_idle(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        m->_punching = false;
        m->_ret.clear();

        if(in.keys.keys.Attack1) {
            m->_punching = true;
            m->_changeState(m,WINDUP);
        } else if(in.keys.keys.Attack2) {
            m->_punching = false;
            m->_changeState(m,WINDUP);
        }

        m->_ret.sprite = "Idle";
        return m->_ret;
    };

    const Out& Machine::_windup(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        m->_ret.clear();

        if(m->_punching) {
            if(m->_currentFrame() >= Machine::punchWindupSpeed) {
                m->_changeState(m,FOLLOW);
                
                m->_ret.sprite = "PunchFollow";
                m->_ret.hitStartX = 0;
                m->_ret.hitEndX = MoveMachine::Machine::TILE_WIDTH * 2;
                m->_ret.hitY = MoveMachine::Machine::TILE_HEIGHT * 3 / 2;
                return m->_ret;
            }

            m->_ret.sprite = "PunchWindup";
            return m->_ret;
        } else {
            if(m->_currentFrame() >= Machine::kickWindupSpeed) {
                m->_changeState(m,FOLLOW);

                m->_ret.sprite = "KickFollow";
                m->_ret.hitStartX = 0;
                m->_ret.hitEndX = MoveMachine::Machine::TILE_WIDTH * 3;
                m->_ret.hitY = MoveMachine::Machine::TILE_HEIGHT * 3 / 2;
                return m->_ret;
            }

            m->_ret.sprite = "KickWindup";
            return m->_ret;
        }
    };

    const Out& Machine::_follow(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        m->_ret.clear();

        if(m->_punching) {
            if(m->_currentFrame() >= Machine::punchFollowSpeed) {
                m->_punching = false;
                m->_changeState(m,IDLE);

                m->_ret.sprite = "Idle";
                return m->_ret;
            }

            m->_ret.sprite = "PunchFollow";
            return m->_ret;
        } else {
            if(m->_currentFrame() >= Machine::kickFollowSpeed) {
                m->_punching = false;
                m->_changeState(m,IDLE);

                m->_ret.sprite = "Idle";
                return m->_ret;
            }

            m->_ret.sprite = "KickFollow";
            return m->_ret;
        }
    };

    size_t Machine::serialize(char* addr) {    
        size_t offset = 0;
        State sEnum = (State) _currentEnum();
        offset += serializeBytes(addr+offset, sEnum);
        offset += serializeBytes(addr+offset,_currentFrame());
        offset += serializeBytes(addr+offset,_punching);

        return offset;
    };

    //deserialize
    //size param is modified to increment the size of the object created
    Machine::Machine(char* addr, size_t& size) {
        _initStateMap();
        State sEnum = *reinterpret_cast<State*>(addr+size);
        size += sizeof(sEnum);
        switch(sEnum) {
            case IDLE:
                _changeState(this,IDLE);
                break;
            case WINDUP:
                _changeState(this,WINDUP);
                break;
            case FOLLOW:
                _changeState(this,FOLLOW);
                break;
            default:
                break; //error!
        }

        uint8_t frame = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(frame);
        _changeFrameOnly(frame);

        _punching = *reinterpret_cast<uint8_t*>(addr+size);
        size += sizeof(_punching);
    };

    void Machine::_initStateMap() {
        _enumToStateFunction[IDLE] = _idle;
        _enumToStateFunction[WINDUP] = _windup;
        _enumToStateFunction[FOLLOW] = _follow;
    };
};