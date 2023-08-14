#ifndef CHIP_8_H
#define CHIP_8_H

#include <string>
#include <iostream>


class chip8 {
    private:
        //member variables
        unsigned short opcode;
        unsigned char memory[4096];
        unsigned char V[16];
        unsigned short indexReg;
        unsigned short programCounter;
        unsigned char delayTimer;
        unsigned char soundTimer;
        unsigned short stack[16];
        unsigned short stackPointer;
        static unsigned char chip8_fontset[80];

    public:
        //member variables
        unsigned char screen[64*32];
        unsigned char keyPad[16];
        
        //member functions
        void initialize();
        bool loadFile(const char* fileName);
        void emulateCycle();
};

#endif