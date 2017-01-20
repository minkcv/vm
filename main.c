#include "instruction.h"
#include "opcodes.h"
#include "vm.h"

int main (char argc, char* argv)
{
    // 0xB011: Load the constant "3" into register 0
    // 0x7000: Jump to the address in register 0
    // 0x0000: Halt (except we skip over this instruction)
    // 0xB206: Load the constant "6" into register 2
    // 0x2101: Subtract the values in registers 1 and 0 and
    // store the result into register 1
    // 0x4120: Jumps to the address in register 2 if the value 
    // in register 1 is a less than
    // 0x9200: Stores the value in register 2 in memory segment 0, cell 0
    // 0x0000: Halt
    int code[8] = { 0xB003, 0x7000, 0x0000, 0xB206, 0x2101, 0x4120, 0x9200, 0x0000 };
    VM* vm = createVM(code);
    run(vm);
    return 0;
}
