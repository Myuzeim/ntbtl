#ifndef ATTACK_MACHINE_H
#define ATTACK_MACHINE_H

#include "state_machine.h"
#include "input_machine.h"
#include <string>

namespace AttackMachine {
    struct In {
        InputMachine::Out keys;
    };
    struct Out {
        std::string sprite;
        uint8_t hitStartX;
        uint8_t hitEndX;
        uint8_t hitY;
    };

    class Machine : public StateMachine<In,Out> {
        private:
            //consts
            //higher = slower
            static const uint8_t punchWindupSpeed = 3;
            static const uint8_t punchFollowSpeed = 6;
            static const uint8_t kickWindupSpeed = 6;
            static const uint8_t kickFollowSpeed = 9;

            //vars
            bool _punching;

            // state functions
            static const Out _idle(StateMachine<In,Out>* sm, const In& in);
            static const Out _windup(StateMachine<In,Out>* sm, const In& in);
            static const Out _follow(StateMachine<In,Out>* sm, const In& in);
        public:
            Machine();
            void reset();
    };
};

#endif //ATTACK_MACHINE_H