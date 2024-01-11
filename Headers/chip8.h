#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>
#include <iostream>
#include <cstring>

class chip8 {

	public:
	//attributes
	//memory
	uint8_t registers[16];
	uint8_t memory[4096]; 
	
	//stack and regs
	uint16_t indexReg;
	uint16_t pc;
	uint16_t stack[16];
	uint8_t sp;
	
	//timers
	uint8_t delay;
	uint8_t sound;
	
	//i/o
	uint8_t keys[16];
	uint32_t screen[64*32];
	bool drawFlag;

	uint16_t opcode;

	//constructor 
	chip8();

	//methods
	void load(char const* file);
	void cpuCycle();

	private:
	//OPCodes
	void op_00e0();
	void op_00ee();
	void op_1nnn();
	void op_2nnn();
	void op_3xkk();
	void op_4xkk();
	void op_5xy0();
	void op_6xkk();
	void op_7xkk();
	void op_8xy0();
	void op_8xy1();
	void op_8xy2();
	void op_8xy3();
	void op_8xy4();
	void op_8xy5();
	void op_8xy6();
	void op_8xy7();
	void op_8xyE();
	void op_9xy0();
	void op_Annn();
	void op_Bnnn();
	void op_Cxkk();
	void op_Dxyn();
	void op_Ex9E();
	void op_ExA1();
	void op_Fx07();
	void op_Fx0A();
	void op_Fx15();
	void op_Fx18();
	void op_Fx1E();
	void op_Fx29();
	void op_Fx33();
	void op_Fx55();
	void op_Fx65();
	void op_null();
};

#endif
