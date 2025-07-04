#ifndef KEY_READER_H
#define KEY_READER_h

#include <fstream>
#include <cstdint>
#include "input_machine.h"

class KeyReader {
    private:
        static const size_t LINE_LENGTH = 12;
        std::ifstream f;
        uint32_t repeats;
        InputMachine::In buffer;
    public:
        void init(const char * filePath) {
            f.open(filePath);
            feed();
        };
        InputMachine::In line() {
            InputMachine::In ret = buffer;
            if(repeats > 0) {
                repeats--;
            } else {
                feed();
            }
            return ret;

        };
        void finish() {
            f.close();
        };
    private:
        void feed() {
            repeats = 0;
            char reg;
            f.readsome(&reg,1);
            repeats += reg;
            repeats <<= 8;
            f.readsome(&reg,1);
            repeats += reg;
            repeats <<= 8;
            f.readsome(&reg,1);
            repeats += reg;
            repeats <<= 8;
            f.readsome(&reg,1);
            repeats += reg;
            f.readsome(reinterpret_cast<char*>(&buffer),sizeof(buffer));
        };
        
};

#endif //KEY_READER_H