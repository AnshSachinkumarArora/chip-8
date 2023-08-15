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
  this->programCounter = 0x200;
  this->opcode = 0;
  this->indexReg = 0;
  this->stackPointer = 0;

  for(int i = 0; i < sizeof(this->memory); i++) {
    this->memory[i] = 0;
  }

  for(int i = 0; i < sizeof(this->stack); i++) {
    this->stack[i] = 0;
  }

  for(int i = 0; i < sizeof(this->screen); i++) {
    this->screen[i] = 0;
  }

  for(int i = 0; i < sizeof(this->registers); i++) {
    this->registers[i] = 0;
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
    this->memory[512+i] = (unsigned char)buffer[i];
  }

  fclose(file);
  free(buffer);

  return true;
}

void chip8::emulateCycle() {
  this->opcode = this->memory[this->programCounter] <<8 | this->memory[this->programCounter + 1];

  switch(this->opcode & 0xF000) {
    case 0x0000:
      switch(this->opcode & 0x000F) {
        //00E0
        case 0x0000:
          for(int i = 0; i < sizeof(this->screen); i++) {
            this->screen[i] = 0;
          }
          break;
        //00EE
        case 0x000E:
          this->programCounter = this->stack[this->stackPointer];
          this->stackPointer--;
          this->programCounter += 2;
          break;
      }
      break;
    
    case 0x0001:
      //1nnn
      this->programCounter = this->opcode & 0x0FFF;
      break;

    case 0x0002:
      //2nnn
      this->stack[this->stackPointer] = this->programCounter;
      this->stackPointer++;
      this->programCounter = this->opcode & 0x0FFF;
      break;

    case 0x0003:
      //3xkk
      if(this->registers[(this->opcode & 0x0F00) >> 8] == this->opcode & 0x00FF) {
        this->programCounter += 4;
      } else {
        this->programCounter += 2;
      }
  }
}