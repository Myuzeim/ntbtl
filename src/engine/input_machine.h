#ifndef INPUT_MACHINE_H
#define INPUT_MACHINE_H

#include "state_machine.h"

#include <memory>
#include <functional>
#include <cstring>

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
            
            // state enum
            enum State : uint8_t {
                UNIVERSAL
            };
        public:
            Machine() {
                _changeState(this, _universal, UNIVERSAL);
            };

            const Out step(const In& in) {
                StateMachine::step(in);
                Out ret;
                ret.keys = in;
                return ret;
            };

            size_t serialize(char* addr) {
                size_t offset = 0;
                State sEnum = (State) _currentEnum();
                memcpy(addr,&sEnum,sizeof(sEnum));
                offset+=sizeof(sEnum);

                uint8_t frame = _currentFrame();
                memcpy(addr+offset,&frame,sizeof(frame));
                offset+=sizeof(frame);

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
            };
    };
}

#endif //INPUT_MACHINE_H