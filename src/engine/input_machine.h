#ifndef INPUT_MACHINE_H
#define INPUT_MACHINE_H

#include "state_machine.h"

#include <memory>
#include <functional>

namespace InputMachine {
    struct In {
        bool Move1;
        bool Move2;
        bool Move3;
        bool Move4;
        bool Move6;
        bool Move7;
        bool Move8;
        bool Move9;
        bool Attack1;
        bool Attack2;
        bool Attack3;
        bool Attack4;
    };

    struct Out {
        In keys;
        //include commands
        bool Back() const { return keys.Move1 || keys.Move4 || keys.Move7;};
        bool Vert() const { return keys.Move2 || keys.Move8;};
        bool Forw() const { return keys.Move3 || keys.Move6 || keys.Move9;};
        bool Far() const { return keys.Move7 || keys.Move8 || keys.Move9;};
        bool Near() const { return keys.Move1 || keys.Move2 || keys.Move3;};
        bool UnMove() const {return !(Back() || Vert() || Forw());};
    };

    
    class Machine : public StateMachine<In,Out> {
        private:
            //state functions
            static Out _universal(StateMachine<In,Out>* sm, const In& in) {return {};};
        public:
            Machine() {
                _changeState(this, _universal);
            };

            const Out step(const In& in) {
                StateMachine::step(in);
                Out ret;
                ret.keys = in;
                return ret;
            };
    };
}

#endif //INPUT_MACHINE_H