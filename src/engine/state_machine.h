#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <cstdint>
#include <functional>

template<typename In, typename Out>
class StateMachine {
    private:
        uint8_t _curFrame;
        
        
    protected:
        typedef std::function<Out(StateMachine*,const In&)> State;
        uint8_t _currentFrame() {return _curFrame;};
        static void _changeState(StateMachine* m, const State& s) {
            m->_state = s;
            m->_curFrame = 0;
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