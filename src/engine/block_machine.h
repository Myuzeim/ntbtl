#ifndef BLOCK_MACHINE_H
#define BLOCK_MACHINE_H

#include "input_machine.h"

namespace BlockMachine {
    struct In {
        InputMachine::Out keys;
        bool movingBack;
    };
    struct Out {
        bool blocking;
    };

    class Machine : public StateMachine<In,Out> {
        private:
            // state functions
            static const Out _universal(StateMachine<In,Out>* sm, const In& in) {
                return {in.keys.Back() || in.movingBack};
            };
        public:
            Machine() {
                _changeState(this,_universal);
            };
    };
};

#endif //BLOCK_MACHINE_H