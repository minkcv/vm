#include "vm.h"

VM* createVM(
        int* code,
        int pc) {
    VM* vm = (VM*)malloc(sizeof(VM));
    vm->code = code;
    vm->pc = pc;
    return vm;
}
