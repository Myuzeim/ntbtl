#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <cstdint>
#include <functional>

template<typename In, typename Out>
class StateMachine {
    private:
        uint8_t _curFrame;
        uint8_t _stateEnum;
        
    protected:
        typedef std::function<Out(StateMachine*,const In&)> State;
        uint8_t _currentFrame() {return _curFrame;};
        uint8_t _currentEnum() {return _stateEnum;};

        // The "stateEnum" is for serialization (it should map to s somewhere)
        // Otherwise just call "s"
        static void _changeState(StateMachine* m, const State& s, uint8_t stateEnum) {
            m->_state = s;
            m->_curFrame = 0;
            m->_stateEnum = stateEnum;
        };

        //use to change ONLY the frame count. Only use during deserialization!
        void _changeFrameOnly(const uint8_t& c) {
            _curFrame = c;
        };
        
    private:
        State _state;
    public:
        Out step(const In& in) { 
            _curFrame++; 
            return _state(this, in);
        };
};



#endif //STATE_MACHINE_H