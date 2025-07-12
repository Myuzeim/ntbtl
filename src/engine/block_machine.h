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
        void clear() {
            blocking = false;
        }
    };

    class Machine : public StateMachine<In,Out> {
        private:
            //vars
            Out _ret;

            // state functions
            static const Out& _universal(StateMachine<In,Out>* sm, const In& in) {
                Machine* m = reinterpret_cast<Machine*>(sm);
                m->_ret = {in.keys.Back() || in.movingBack};
                return m->_ret;
            };

            // state enums
            enum State : uint8_t {
                UNIVERSAL
            };

            // helper
            void _initStateMap() {
                _enumToStateFunction[UNIVERSAL] = _universal;
            }
        public:
            Machine() {
                _initStateMap();
                _changeState(this,UNIVERSAL);
            };

            size_t serialize(char* addr) {
                size_t offset = 0;
                State sEnum = (State) _currentEnum();
                offset += serializeBytes(addr+offset, sEnum);
                offset += serializeBytes(addr+offset,_currentFrame());

                return offset;
            }

            //deserialize
            //size param is modified to increment the size of the object created
            Machine(char* addr, size_t& size) {
                _initStateMap();
                State sEnum = *reinterpret_cast<State*>(addr+size);
                size += sizeof(sEnum);
                switch(sEnum) {
                    case UNIVERSAL:
                        _changeState(this,UNIVERSAL);
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