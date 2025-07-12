#ifndef MOVE_MACHINE_H
#define MOVE_MACHINE_H

#include "state_machine.h"
#include "input_machine.h"
#include <functional>

//Describes the position of the character and if it is in movement
namespace MoveMachine {
    struct In {
        InputMachine::Out keys;
        uint8_t px, py;
    };  
    struct Out {
        uint8_t px, py;
        bool inMove;
        bool movingBack;
        void clear() {
            px = 0;
            py = 0;
            inMove = false;
            movingBack = false;
        }
    };
    
    class Machine : public StateMachine<In,Out> {
        private:
            //consts
            //higher = slower
            static const uint8_t VERT_SPEED = 5;
            static const uint8_t HORI_SPEED = 10;
            static const uint8_t DIAG_SPEED = 12;

            //vars
            uint8_t _moveFrames;
            uint8_t _srcX;
            uint8_t _srcY;
            uint8_t _dstX;
            uint8_t _dstY;
            bool _movingBack;
            Out _ret;


            // state functions
            static const Out& _idle(StateMachine<In,Out>* sm, const In& in);
            static const Out& _moving(StateMachine<In,Out>* sm, const In& in);

            // state enum
            enum State : uint8_t {
                IDLE,
                MOVING
            };
            
            //helpers
            bool _canMoveFar(const uint8_t& py) {return (py < (TILE_HEIGHT / 2) + TILE_HEIGHT * 2);};
            bool _canMoveNear(const uint8_t& py) {return (py > TILE_HEIGHT / 2);};
            bool _canMoveForw(const uint8_t& px) {return (px < (TILE_WIDTH / 2) + TILE_WIDTH);};
            bool _canMoveBack(const uint8_t& px) {return (px > TILE_WIDTH / 2);};
            void _initStateMap();

        public:
            static const uint8_t TILE_WIDTH = 60;
            static const uint8_t TILE_HEIGHT = 80;
            Machine();
            size_t serialize(char* addr);

            //deserialize
            //size param is modified to increment the size of the object created
            Machine(char* addr, size_t& size);
    };
};

#endif //MOVE_MACHINE_H