#include "instruction.h"
#include "opcodes.h"
#include "vm.h"

int main (char argc, char* argv)
{
    // 0xB011: Load the constant "17" into register 0
    // 0x1001: Add the values in registers 0 and 0 and
    // store the result into register 1
    // 0x0000: Halt
    int code[3] = { 0xB011, 0x1001, 0x0000 };
    VM* vm = createVM(code);
    run(vm);
    return 0;
}
