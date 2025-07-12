#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <cstdint>
#include <functional>
#include <cstring>
#include <unordered_map>

template<typename In, typename Out>
class StateMachine {
    private:
        // serializable
        uint8_t _curFrame;
        uint8_t _stateEnum;
        
        
    protected:
        typedef std::function<const Out&(StateMachine*,const In&)> State;
        uint8_t _currentFrame() {return _curFrame;};
        uint8_t _currentEnum() {return _stateEnum;};

        // The "stateEnum" is for serialization (it should map to an "s" somewhere)
        // The state function "s" will actually be called
        static void _changeState(StateMachine* m, uint8_t stateEnum) {
            m->_curFrame = 0;
            m->_stateEnum = stateEnum;
        };

        //use to change ONLY the frame count. Only use during deserialization!
        void _changeFrameOnly(const uint8_t& c) {
            _curFrame = c;
        };

        virtual void _initStateMap() = 0;
        
        // nonserializable
        std::unordered_map<uint8_t,State> _enumToStateFunction;

    public:
        // "Out" is live ONLY until you call step next.
        const Out& step(const In& in) { 
            _curFrame++; 
            return _enumToStateFunction[_stateEnum](this, in);
        };

        // Child classes should also implement a "deserialize" constructor with signature like:
        // Machine(char* addr, size_t size)
        // It should mirror this function
        // size_t in both cases refers to an offset.
        virtual size_t serialize(char* addr) = 0;

    protected:
        // helper functions

        //returns amount to increment offset
        template<typename T>
        size_t serializeBytes(char* addr, T memberVariable) {
            memcpy(addr,&memberVariable,sizeof(T));
            return sizeof(T);
        }
};



#endif //STATE_MACHINE_H