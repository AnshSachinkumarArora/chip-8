Chip-8 is an interpreted programming language developed by Joseph Weisbecker. 
Chip-8 programs run using a Chip-8 Virtual Machine and was initially used to make game programming easier.
This Chip-8 emulator is written in C++.

Test ROM:
![test rom](https://github.com/AnshSachinkumarArora/chip-8/blob/main/Images/Chip-8%20test%20photo.PNG)

Brix:
![brix game image](https://github.com/AnshSachinkumarArora/chip-8/blob/main/Images/chip-8%20brix.PNG)

Requires CMake and SDL/SDL2:

Steps to compile:
```
mkdir build
cd build
cmake ..
make
```

Steps to run:
```
./chip8 ../ROMS/<insert rom name>
```
