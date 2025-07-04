#include "attack_machine.h"
#include "move_machine.h"

namespace AttackMachine {
    Machine::Machine() {
        _punching = false;
        _changeState(this,_idle);
    };

    const Out Machine::_idle(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        m->_punching = false;
        if(in.keys.keys.Attack1) {
            m->_punching = true;
            m->_changeState(m,_windup);
        } else if(in.keys.keys.Attack2) {
            m->_punching = false;
            m->_changeState(m,_windup);
        }
        return {"Idle",0,0,0};
    };

    const Out Machine::_windup(StateMachine<In,Out>* sm, const In& in) {
        Machine* m = static_cast<Machine*>(sm);
        if(m->_punching) {
            if(m->_currentFrame() >= Machine::punchWindupSpeed) {
                m->_changeState(m,_follow);
                return {"PunchFollow",0,MoveMachine::Machine::TILE_WIDTH * 2,MoveMachine::Machine::TILE_HEIGHT * 3 / 2};
            }
            return {"PunchWindup",0,0,0};
        } else {
            if(m->_currentFrame() >= Machine::kickWindupSpeed) {
                m->_changeState(m,_follow);
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
                m->_changeState(m,_idle);
                return {"Idle",0,0,0};
            }
            return {"PunchFollow",0,0};
        } else {
            if(m->_currentFrame() >= Machine::kickFollowSpeed) {
                m->_punching = false;
                m->_changeState(m,_idle);
                return {"Idle",0,0,0};
            }
            return {"KickFollow",0,0};
        }
    };
};