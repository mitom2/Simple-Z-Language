
# Simple-Z-Language (SZL)

Simple-Z-Language (SZL) is a programming language designed with retro-style devices in mind. SZL features an intuitive syntax for data structures (called objects), memory management, dynamic data manipulation, I/O operations, and more.
## Compiler Features

- Memory Management: SZL allows for fine-grained memory control with alloc() and free().
- Struct-Oriented Syntax: Defines objects and functions in a simple, C-like style.
- Type Safety: Strong typing for containers and elements, explicit type casting required in all cases (no implicit conversions).
- Simple preprocessor.
- Basic data-structures available as includable libraries.
## Syntax
### Objects
In SZL, objects are defined with the object keyword, followed by the object name and its member fields. For example:

```szl
object Car
{
    uint mass;
    ulong mileage;
    uint fuel;
}
```
### Functions
Functions are defined with the void or return type (e.g., uint) followed by the function name, parameters, and function body.

```szl
Car carCreate(uint defMass)
{
    Car res;
    res.mass = defMass;
    res.mileage = 0;
    res.fuel = 1.0;
    return res;
}
```
### Memory Allocation and Deallocation
SZL provides a simple memory management system using alloc() for allocating memory and free() for deallocating it. These are used for dynamic memory management of objects and their data.

```szl
uint addr = alloc(sizeof(Car));
free(addr, sizeof(Car));
```

### Pointers
Pointers are not a separate type. Instead, uint variable is used to hold the memory address.

```szl
uint addr = alloc(sizeof(int));
int val = 53;
uint addr2 = ?@ val; // Gets memory address of val. //
val -> addr; // Writes 53 to memory addressed by addr. //
20 -> addr2; // Writes 20 to val. //
val = int @ addr; // Gets int value from memory location addressed by addr. //
```
## Installation

To use SZL, follow these steps:

```bash
git clone https://github.com/mitom2/Simple-Z-Language.git
cd Simple-Z-Language
mkdir build
cd build
cmake ..
make
```
## Usage/Examples
Compiler usage:
```bash
./Simple-Z-Language -i main.szl -o output.bin
```
After this command is executed, two files will be created: main.szl.ia and output.bin. First one contains code in [SZAMAN](https://github.com/mitom2/SZAMAN) and second one machine code ready to be installed to the device (or [emulator](https://github.com/mitom2/ZOE)).

Examples of syntax and programs can be found in [the examples directory](https://github.com/mitom2/Simple-Z-Language/tree/development/examples).
## License
This project is licensed under the terms of the MIT license. Check the LICENSE file for details.