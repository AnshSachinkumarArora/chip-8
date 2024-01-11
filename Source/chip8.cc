#include "../Headers/chip8.h"

//constructor
chip8::chip8(){

	//fontset
	uint8_t fontset[80] = 
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, 
		0x20, 0x60, 0x20, 0x20, 0x70, 
		0xF0, 0x10, 0xF0, 0x80, 0xF0, 
		0xF0, 0x10, 0xF0, 0x10, 0xF0, 
		0x90, 0x90, 0xF0, 0x10, 0x10, 
		0xF0, 0x80, 0xF0, 0x10, 0xF0, 
		0xF0, 0x80, 0xF0, 0x90, 0xF0, 
		0xF0, 0x10, 0x20, 0x40, 0x40, 
		0xF0, 0x90, 0xF0, 0x90, 0xF0,
		0xF0, 0x90, 0xF0, 0x10, 0xF0,
		0xF0, 0x90, 0xF0, 0x90, 0x90,
		0xE0, 0x90, 0xE0, 0x90, 0xE0,
		0xF0, 0x80, 0x80, 0x80, 0xF0,
		0xE0, 0x90, 0x90, 0x90, 0xE0,
		0xF0, 0x80, 0xF0, 0x80, 0xF0,
		0xF0, 0x80, 0xF0, 0x80, 0x80 
	};

	pc      = 0x200;    // Set program counter to 0x200
    opcode  = 0;        // Reset op code
    indexReg     = 0;          // Reset I
    sp      = 0;        // Reset stack pointer

    // Clear the display
    for (int i = 0; i < 2048; ++i) {
        screen[i] = 0;
    }

    // Clear the stack, keypad, and V registers
    for (int i = 0; i < 16; ++i) {
        stack[i]    = 0;
        keys[i]      = 0;
        registers[i]        = 0;
    }

    // Clear memory
    for (int i = 0; i < 4096; ++i) {
        memory[i] = 0;
    }

    // Load font set into memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = fontset[i];
    }

    // Reset timers
    delay = 0;
    sound = 0;

    // Seed rng
    srand (time(NULL));
}

//load rom into memory
void chip8::load(const char* file) {
	std::ifstream rom(file, std::ios::binary | std::ios::ate);

	if(rom.is_open()) {
		//get size of file and create buffer
		auto size = rom.tellg();
		char* buffer = new char[size];

		//read file contents into the buffer
		rom.seekg(0, std::ios::beg);
		rom.read(buffer, size);
		rom.close();

		//load buffer contents into memory
		for(long i = 0; i < size; ++i) {
			memory[0x200 + i] = buffer[i];
		}

		delete[] buffer;
	}
}

void chip8::cpuCycle() {
    // Fetch op code
    opcode = memory[pc] << 8 | memory[pc + 1];   // Op code is two bytes

    switch(opcode & 0xF000){
        // 00E_
        case 0x0000:
            
            switch (opcode & 0x000F) {
                // 00E0 - Clear screen
                case 0x0000:
                    for (int i = 0; i < 2048; ++i) {
                        screen[i] = 0;
                    }
                    drawFlag = true;
                    pc+=2;
                    break;

                // 00EE - Return from subroutine
                case 0x000E:
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;

                default:
                    printf("\nUnknown op code: %.4X\n", opcode);
                    exit(3);
            }
            break;

        // 1NNN - Jumps to address NNN
        case 0x1000:
            pc = opcode & 0x0FFF;
            break;

        // 2NNN - Calls subroutine at NNN
        case 0x2000:
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        // 3XNN - Skips the next instruction if VX equals NN.
        case 0x3000:
            if (registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;

        // 4XNN - Skips the next instruction if VX does not equal NN.
        case 0x4000:
            if (registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;

        // 5XY0 - Skips the next instruction if VX equals VY.
        case 0x5000:
            if (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;

        // 6XNN - Sets VX to NN.
        case 0x6000:
            registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;

        // 7XNN - Adds NN to VX.
        case 0x7000:
            registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;

        // 8XY_
        case 0x8000:
            switch (opcode & 0x000F) {

                // 8XY0 - Sets VX to the value of VY.
                case 0x0000:
                    registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY1 - Sets VX to (VX OR VY).
                case 0x0001:
                    registers[(opcode & 0x0F00) >> 8] |= registers[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY2 - Sets VX to (VX AND VY).
                case 0x0002:
                    registers[(opcode & 0x0F00) >> 8] &= registers[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY3 - Sets VX to (VX XOR VY).
                case 0x0003:
                    registers[(opcode & 0x0F00) >> 8] ^= registers[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry,
                // and to 0 when there isn't.
                case 0x0004:
                    registers[(opcode & 0x0F00) >> 8] += registers[(opcode & 0x00F0) >> 4];
                    if(registers[(opcode & 0x00F0) >> 4] > (0xFF - registers[(opcode & 0x0F00) >> 8]))
                        registers[0xF] = 1; //carry
                    else
                        registers[0xF] = 0;
                    pc += 2;
                    break;

                // 8XY5 - VY is subtracted from VX. VF is set to 0 when
                // there's a borrow, and 1 when there isn't.
                case 0x0005:
                    if(registers[(opcode & 0x00F0) >> 4] > registers[(opcode & 0x0F00) >> 8])
                        registers[0xF] = 0; // there is a borrow
                    else
                        registers[0xF] = 1;
                    registers[(opcode & 0x0F00) >> 8] -= registers[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;

                // 0x8XY6 - Shifts VX right by one. VF is set to the value of
                // the least significant bit of VX before the shift.
                case 0x0006:
                    registers[0xF] = registers[(opcode & 0x0F00) >> 8] & 0x1;
                    registers[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;

                // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's
                // a borrow, and 1 when there isn't.
                case 0x0007:
                    if(registers[(opcode & 0x0F00) >> 8] > registers[(opcode & 0x00F0) >> 4])	// VY-VX
                        registers[0xF] = 0; // there is a borrow
                    else
                        registers[0xF] = 1;
                    registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                // 0x8XYE: Shifts VX left by one. VF is set to the value of
                // the most significant bit of VX before the shift.
                case 0x000E:
                    registers[0xF] = registers[(opcode & 0x0F00) >> 8] >> 7;
                    registers[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;

                default:
                    printf("\nUnknown op code: %.4X\n", opcode);
                    exit(3);
            }
            break;

        // 9XY0 - Skips the next instruction if VX doesn't equal VY.
        case 0x9000:
            if (registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;

        // ANNN - Sets indexReg to the address NNN.
        case 0xA000:
            indexReg = opcode & 0x0FFF;
            pc += 2;
            break;

        // BNNN - Jumps to the address NNN plus V0.
        case 0xB000:
            pc = (opcode & 0x0FFF) + registers[0];
            break;

        // CXNN - Sets VX to a random number, masked by NN.
        case 0xC000:
            registers[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
            pc += 2;
            break;

        // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8
        // pixels and a height of N pixels.
        // Each row of 8 pixels is read as bit-coded starting from memory
        // location indexReg;
        // indexReg value doesn't change after the execution of this instruction.
        // VF is set to 1 if any screen pixels are flipped from set to unset
        // when the sprite is drawn, and to 0 if that doesn't happen.
        case 0xD000:
        {
            unsigned short x = registers[(opcode & 0x0F00) >> 8];
            unsigned short y = registers[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            registers[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[indexReg + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(screen[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            registers[0xF] = 1;
                        }
                        screen[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        }
            break;

        // EX__
        case 0xE000:

            switch (opcode & 0x00FF) {
                // EX9E - Skips the next instruction if the keys stored
                // in VX is pressed.
                case 0x009E:
                    if (keys[registers[(opcode & 0x0F00) >> 8]] != 0)
                        pc +=  4;
                    else
                        pc += 2;
                    break;

                // EXA1 - Skips the next instruction if the keys stored
                // in VX isn't pressed.
                case 0x00A1:
                    if (keys[registers[(opcode & 0x0F00) >> 8]] == 0)
                        pc +=  4;
                    else
                        pc += 2;
                    break;

                default:
                    printf("\nUnknown op code: %.4X\n", opcode);
                    exit(3);
            }
            break;

        // FX__
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                // FX07 - Sets VX to the value of the delay timer
                case 0x0007:
                    registers[(opcode & 0x0F00) >> 8] = delay;
                    pc += 2;
                    break;

                // FX0A - A keys press is awaited, and then stored in VX
                case 0x000A:
                {
                    bool keys_pressed = false;

                    for(int i = 0; i < 16; ++i)
                    {
                        if(keys[i] != 0)
                        {
                            registers[(opcode & 0x0F00) >> 8] = i;
                            keys_pressed = true;
                        }
                    }

                    // If no keys is pressed, return and try again.
                    if(!keys_pressed)
                        return;

                    pc += 2;
                }
                    break;

                // FX15 - Sets the delay timer to VX
                case 0x0015:
                    delay = registers[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                // FX18 - Sets the sound timer to VX
                case 0x0018:
                    sound = registers[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                // FX1E - Adds VX to indexReg
                case 0x001E:
                    // VF is set to 1 when range overflow (indexReg+VX>0xFFF), and 0
                    // when there isn't.
                    if(indexReg + registers[(opcode & 0x0F00) >> 8] > 0xFFF)
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    indexReg += registers[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;

                // FX29 - Sets indexReg to the location of the sprite for the
                // character in VX. Characters 0-F (in hexadecimal) are
                // represented by a 4x5 font
                case 0x0029:
                    indexReg = registers[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;

                // FX33 - Stores the Binary-coded decimal representation of VX
                // at the addresses indexReg, indexReg plus 1, and indexReg plus 2
                case 0x0033:
                    memory[indexReg]     = registers[(opcode & 0x0F00) >> 8] / 100;
                    memory[indexReg + 1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[indexReg + 2] = registers[(opcode & 0x0F00) >> 8] % 10;
                    pc += 2;
                    break;

                // FX55 - Stores V0 to VX in memory starting at address indexReg
                case 0x0055:
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        memory[indexReg + i] = registers[i];

                    // On the original interpreter, when the
                    // operation is done, indexReg = indexReg + X + 1.
                    indexReg += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;

                case 0x0065:
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        registers[i] = memory[indexReg + i];

                    // On the original interpreter,
                    // when the operation is done, indexReg = indexReg + X + 1.
                    indexReg += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;

                default:
                    printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
            break;

        default:
            printf("\nUnimplemented op code: %.4X\n", opcode);
            exit(3);
    }


    // Update timers
    if (delay > 0)
        --delay;

    if (sound > 0)
        if(sound == 1);
            // TODO: Implement sound
        --sound;


}