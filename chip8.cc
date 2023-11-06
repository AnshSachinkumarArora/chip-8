#include "chip8.h"

unsigned char fontset[80] = 
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

chip8::chip8() {};
chip8::~chip8() {};

void chip8::initialize() {
  programCounter = 0x200;
  opcode = 0;
  indexReg = 0;
  stackPointer = 0;
  delayTimer = 0;
  soundTimer = 0;
  draw = true;

  srand(time(NULL));

  for(int i = 0; i < 4096; i++) {
    memory[i] = 0;
  }

  for(int i = 0; i < 16; i++) {
    stack[i] = 0;
  }

  for(int i = 0; i < 2048; i++) {
    screen[i] = 0;
  }

  for(int i = 0; i < 16; i++) {
    registers[i] = 0;
  }

  for(int i = 0; i < 16; i++) {
    keyPad[i] = 0;
  }

  for(int i = 0; i < 80; i++) {
    memory[i] = fontset[i];
  }

  std::cout<<"init done"<<std::endl;
}

bool chip8::loadFile(const char* fileName) {
  initialize();

  FILE* file = fopen(fileName, "rb");

  if(file == NULL) {
    std::cout<<"Failed to load file"<<std::endl;
    return false;
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*) malloc(sizeof(char) * fileSize);

  for(int i = 0; i < fileSize; i++) {
    memory[512+i] = (uint8_t)buffer[i];
  }

  fclose(file);
  free(buffer);

  std::cout<<"loading done"<<std::endl;

  return true;
}

void chip8::emulateCycle() {
  opcode = memory[programCounter] <<8 | memory[programCounter + 1];

  switch(opcode & 0xF000) {
    case 0x0000:
      std::cout<<"0x0"<<std::endl;
      switch(opcode & 0x000F) {
        //00E0
        case 0x0000:
          for(int i = 0; i < 2048; i++) {
            screen[i] = 0;
          }
          draw = true;
          programCounter += 2;
          break;
        //00EE
        case 0x000E:
          stackPointer--;
          programCounter = stack[stackPointer];
          programCounter += 2;
          break;
      }
      break;
    
    case 0x1000:
      std::cout<<"0x1"<<std::endl;
      //1nnn
      programCounter = opcode & 0x0FFF;
      break;

    case 0x2000:
      std::cout<<"0x2"<<std::endl;
      //2nnn
      stack[stackPointer] = programCounter;
      stackPointer++;
      programCounter = opcode & 0x0FFF;
      break;

    case 0x3000:
      std::cout<<"0x3"<<std::endl;
      //3xkk
      if(registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;

    case 0x4000:
      std::cout<<"0x4"<<std::endl;
      //4xkk
      if(registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;

    case 0x5000:
      std::cout<<"0x5"<<std::endl;
      //5xy0
      if(registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4]) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;

    case 0x6000:
      std::cout<<"0x6"<<std::endl;
      //6xkk
      registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
      programCounter += 2;
      break;

    case 0x7000:
      std::cout<<"0x7"<<std::endl;
      //7xkk
      registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] + opcode & 0x00FF;
      programCounter += 2;
      break;

    case 0x8000:
      std::cout<<"0x8"<<std::endl;
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
        {
          //8xy4
          registers[(opcode & 0x0F00) >> 8] += registers[(opcode & 0x00F0) >> 4];
          if(registers[(opcode & 0x00F0) >> 4] > (0xFF - registers[(opcode & 0x0F00) >> 8])) {
            registers[0x000F] = 1;
          } else {
            registers[0x000F] = 0;
          }
          programCounter += 2;
          break;
        }

        case 0x0005:
          //8xy5
          if(registers[(opcode & 0x00F0) >> 4] > registers[(opcode & 0x0F00) >> 8]) {
            registers[0x000F] = 0;
          } else {
            registers[0x000F] = 1;
          }
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] - registers[(opcode & 0x00F0) >> 4];
          programCounter += 2;
          break;

        case 0x0006:
          //8xy6
          registers[0xf] = registers[(opcode & 0x0F00) >> 8] & 0x0001;
          registers[(opcode & 0x0F00) >> 8] >>= 1;
          programCounter += 2;
          break;
        
        case 0x0007:
          //8xy7
          if(registers[(opcode & 0x0F00) >> 8] < registers[(opcode & 0x00F0) >> 4]) {
            registers[0x000F] = 1;
          } else {
            registers[0x000F] = 0;
          }
          registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];
          programCounter += 2;
          break;

        case 0x000E:
          //8xyE
          registers[0xF] = registers[(opcode & 0x0F00) >> 8] >> 7;
          registers[(opcode & 0x0F00) >> 8] <<= 1;
          programCounter += 2;
          break;
      }
      break;

    case 0x9000:
      std::cout<<"0x9"<<std::endl;
      //9xy0
      if(registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4]) {
        programCounter += 4;
      } else {
        programCounter += 2;
      }
      break;

    case 0xA000:
      std::cout<<"0xA"<<std::endl;
      //Annn
      indexReg = opcode & 0x0FFF;
      programCounter += 2;
      break;
    
    case 0xB000:
      std::cout<<"0xB"<<std::endl;
      //Bnnn
      programCounter = (opcode & 0x0FFF) + registers[0x0000];
      break;

    case 0xC000:
      std::cout<<"0xC"<<std::endl;
      //Cxkk
      registers[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
      programCounter += 2;
      break;

    case 0xD000:
    std::cout<<"0xD"<<std::endl;
    {
      //Dxyn
      unsigned short x = registers[(opcode & 0x0F00) >> 8];
      unsigned short y = registers[(opcode & 0x00F0) >> 4];
      unsigned short n = opcode & 0x000F;
      unsigned short pix;
      registers[0x000F] = 0;

      for(int i = 0; i < n; i++) {
        pix = memory[indexReg + i];
        for(int j = 0; j < 8; j++) {
          if((pix & (0x0080 >> j)) != 0) {
            if(screen[(x + j + ((y + i) * 64)) == 1]) {
              registers[0x000F] = 1;
            }
            screen[(x + j + ((y + i) * 64))] ^= 1;
          }
        }
      }
      draw = true;
      programCounter += 2;
      break;
    }

    case 0xE000:
      std::cout<<"0xE"<<std::endl;
      //Ex**
      switch(opcode & 0x00FF) {
        case 0x009E:
          //Ex9E
          if(keyPad[registers[(opcode & 0x0F00) >> 8]] != 0) {
            programCounter += 4;
          } else {
            programCounter += 2;
          }
          break;
        
        case 0x00A1:
          //ExA1
          if(keyPad[registers[(opcode & 0x0F00) >> 8]] == 0) {
            programCounter += 4;
          } else {
            programCounter += 2;
          }
          break;
      }
      break;

    case 0xF000:
      std::cout<<"0xF"<<std::endl;
      //Fx**
      switch(opcode & 0x00FF) {
        case 0x0007:
          //Fx07
          registers[(opcode & 0x0F00) >> 8] = delayTimer;
          programCounter += 2;
          break;

        case 0x000A:
        {
          //Fx0A
          bool keyPress = false;
          for(int i = 0; i < 16; i++) {
            if(keyPad[i] == 1) {
              registers[(opcode & 0x0F00) >> 8] = i;
              keyPress = true;
            }
          }
          if(!keyPress) {
            return;
          }
          programCounter += 2;
        }
        break;

        case 0x0015:
          //Fx15
          delayTimer = registers[(opcode & 0x0F00) >> 8];
          programCounter += 2;
          break;

        case 0x0018:
          //Fx18
          soundTimer = registers[(opcode & 0x0F00) >> 8];
          programCounter += 2;
          break;

        case 0x001E:
          //Fx1E
          if(indexReg + registers[(opcode & 0x0F00) >> 8] > 0x0FFF) {
            registers[0x000F] = 1;
          } else {
            registers[0x000F] = 0;
          }
          indexReg += registers[(opcode & 0x0F00) >> 8];
          programCounter += 2;
          break;

        case 0x0029:
          //Fx29
          indexReg = registers[(opcode & 0x0F00) >> 8] * 0x5;
          programCounter += 2;
          break;

        case 0x0033:
          //Fx33
          memory[indexReg] = registers[(opcode & 0x0F00) >> 8] / 100;
          memory[indexReg+1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
          memory[indexReg+2] = registers[(opcode & 0x0F00) >> 8] % 10;
          programCounter += 2;
          break;

        case 0x0055:
          //Fx55
          for(int i = 0; i < ((opcode & 0x0F00) >> 8); i++) {
            memory[indexReg+i] = registers[i];
          }
          indexReg += ((opcode & 0x0F00) >> 8) + 1;
          programCounter += 2;
          break;
        
        case 0x0065:
          for(int i = 0; i < ((opcode & 0x0F00) >> 8); i++) {
            registers[i] = memory[indexReg+i];
          }
          indexReg = ((opcode & 0x0F00) >> 8) + 1;
          programCounter += 2;
          break;        
      }
      break;
  }

  std::cout<<"opcode executed"<<std::endl;

  if(delayTimer > 0) {
    delayTimer--;
  }
  if(soundTimer > 0) {
    if(soundTimer == 1) {
      std::cout<<"Beep"<<std::endl;
    }
    soundTimer--;
  }
}