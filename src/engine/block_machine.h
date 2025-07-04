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

            // state enums
            enum State : uint8_t {
                UNIVERSAL
            };
            
        public:
            Machine() {
                _changeState(this,_universal,UNIVERSAL);
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
            }

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
            };
    };
};

#endif //BLOCK_MACHINE_H