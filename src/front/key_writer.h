#ifndef KEY_WRITER_H
#define KEY_WRITER_H

#include <fstream>
#include <cstdint>
#include "input_machine.h"

class KeyWriter {
    private:
        static const size_t LINE_LENGTH = 12;
        std::ofstream f;
        uint32_t repeats;
        char buffer[LINE_LENGTH];
    public:
        void init(const char * filePath) {
            f.open(filePath,std::ios::trunc);
        };
        void line(const InputMachine::In& in) {
            if(!memcmp(buffer,&in,sizeof(in))) {
                //matched
                repeats++;
            } else {
                flush();
                repeats = 0;
                memcpy(buffer,&in,sizeof(in));
            }
        };
        void finish() {
            flush();
            f.close();
        };
    private:
        void flush() {
            uint32_t reg = repeats & 0xFF000000;
            reg >>= 24;
            char shortReg = reg;
            f.write(&shortReg,1);
            
            reg = repeats & 0x00FF0000;
            reg >>= 16;
            shortReg = reg;
            f.write(&shortReg,1);

            reg = repeats & 0x0000FF00;
            reg >>= 8;
            shortReg = reg;
            f.write(&shortReg,1);

            reg = repeats & 0x000000FF;
            shortReg = reg;
            f.write(&shortReg,1);

            f.write(buffer,LINE_LENGTH);
        };
        
};

#endif //WRITER_H