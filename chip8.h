#ifndef CHIP_8_H
#define CHIP_8_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include "time.h"


class chip8 {
    private:
        //member variables
        uint8_t memory[4096];
        uint8_t registers[16];

        uint16_t opcode;
        uint16_t indexReg;
        uint16_t programCounter;

        uint8_t delayTimer;
        uint8_t soundTimer;

        uint16_t stack[16];
        uint16_t stackPointer;

        void initialize();

    public:
        //member variables
        uint8_t screen[64*32];
        uint8_t keyPad[16];
        bool draw;

        //constructor
        chip8();
        //destructor 
        ~chip8();
        
        //member functions
        bool loadFile(const char* fileName);
        void emulateCycle();
};

#endif