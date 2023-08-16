#include "chip8.h"

unsigned char chip8::chip8_fontset[80] = 
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8::initialize() {
  programCounter = 0x200;
  opcode = 0;
  indexReg = 0;
  stackPointer = 0;

  for(int i = 0; i < sizeof(memory); i++) {
    memory[i] = 0;
  }

  for(int i = 0; i < sizeof(stack); i++) {
    stack[i] = 0;
  }

  for(int i = 0; i < sizeof(screen); i++) {
    screen[i] = 0;
  }

  for(int i = 0; i < sizeof(registers); i++) {
    registers[i] = 0;
  }

  for(int i = 0; i < 80; i++) {
    memory[i] = chip8_fontset[i];
  }
}

bool chip8::loadFile(const char* fileName) {
  FILE* file = fopen(fileName, "rb");

  if(file == NULL) {
    std::cout<<"Failed to load file"<<std::endl;
    return false;
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*) malloc(sizeof(char) * fileSize);
  fread(buffer, sizeof(char), (size_t)fileSize, file);

  for(int i = 0; i < fileSize; i++) {
    memory[512+i] = (unsigned char)buffer[i];
  }

  fclose(file);
  free(buffer);

  return true;
}

void chip8::emulateCycle() {
  opcode = memory[programCounter] <<8 | memory[programCounter + 1];

  switch(opcode & 0xF000) {
    case 0x0000:
      switch(opcode & 0x000F) {
        //00E0
        case 0x0000:
          for(int i = 0; i < sizeof(screen); i++) {
            screen[i] = 0;
          }
          break;
        //00EE
        case 0x000E:
          programCounter = stack[stackPointer];
          stackPointer--;
          programCounter += 2;
          break;
      }
      break;
    
    case 0x0001:
      //1nnn
      programCounter = opcode & 0x0FFF;
      break;

    case 0x0002:
      //2nnn
      stack[stackPointer] = programCounter;
      stackPointer++;
      programCounter = opcode & 0x0FFF;
      break;

    case 0x0003:
      //3xkk
      if(registers[(opcode & 0x0F00) >> 8] == opcode & 0x00FF) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;

    case 0x0004:
      //4xkk
      if(registers[(opcode & 0x0F00) >> 8] != opcode & 0x00FF) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;

    case 0x0005:
      //5xy0
      if(registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4]) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;

    case 0x0006:
      //6xkk
      registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
      programCounter += 2;
      break;

    case 0x0007:
      //7xkk
      registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] + opcode & 0x00FF;
      programCounter += 2;
      break;

    case 0x0008:
      //8xy*
      switch(opcode & 0x000F){
        case 0x0000:
          //8xy0
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
          programCounter += 2;
          break;
        
        case 0x0001:
          //8xy1
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] | registers[(opcode & 0x00F0) >> 4];
          programCounter += 2;
          break;

        case 0x0002:
          //8xy2
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] & registers[(opcode & 0x00F0) >> 4];
          programCounter += 2;
          break;

        case 0x0003:
          //8xy3
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] ^ registers[(opcode & 0x00F0) >> 4];
          programCounter += 2;
          break;

        case 0x0004:
          //8xy4
          unsigned short temp = registers[(opcode & 0x0F00) >> 8] + registers[(opcode & 0x00F0) >> 4];
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] + registers[(opcode & 0x00F0) >> 4];
          if(temp > 0x00FF) {
            registers[0x000F] = 1;
          } else {
            registers[0x000F] = 0;
          }
          programCounter += 2;
          break;
      }
  }
}