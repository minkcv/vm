# Debugging
Run the vm with the debug option to enable breakpoints and debugging features: `./main -f myprogram.vaporbin -d`. 

## Breakpoints
Create a breakpoint with `break;` in vaporlang and `BRKP` in assembly. Break instructions are a no-op when running without the debug option.

## Keyboard controls
- F2: Continue: Resume normal execution
- F3: Break All: Enter break state 
- F4: Step: Execute one instruction
- F9: Print program counter, disassemble current instruction, print register values
- F10: Dump memory segments 0 through 63 (inclusive)
- F11: Dump memory segments 64 through 127 (inclusive)
- F12: Disassemble next 16 instructions

## Understanding the Output
### F9
Program counter: The segment and offset address of the current instruction, first in decimal, then hexidecimal.

    PC:  036.009 0x24.0x09
    
Disassembly: The assembly for the current instruction. Decimal constants include comments for their hexadecimal value.

    ASM: LRC r02 #036 ; (0x0024)
    
Registers: Values of registers in decimal and then hexadecimal

    r00: 001 (0x01)
    r01: 001 (0x01)
    ...

### F10
16 bytes of memory per row (32 hex digits). This example contains the x y locations of pieces in tetris.

    0x00.0x00: 0x070E060F050F070F020E010F000F020F
    0x00.0x10: 0x070C070D060D060E0000000000000000
    ...

Segment 63 contains the return addresses of functions for vaporlang programs. 
The first byte `0x04` is the call stack size in bytes. The next 2 bytes `0x02` and `0x1E` are a segment and offset return address. The next `0x21.0x55` is another return address and explains the callstack size (4 bytes). The highest order return address is the current one at the time of execution.

    0x3F.0x00: 0x04021E21550000000000000000000000
    
Looking up `0x02.0x1E` in the assembly (with instruction address logging) shows that this address is directly after a call to the render function.

    LRL r0 r1 @render              ; 002.027  0x02.0x1B
    JMP r0 r1                      ; 002.029  0x02.0x1D
    LRC r0 #63                     ; 002.030  0x02.0x1E <- The stored return address
    
At `0x21.0x55` is the return address from the renderTile function. The call stack shows that the program has called render and render has called renderTile.

### F12
Disassembly of the next 16 instructions. The * marks the current instruction.

    * LRC r02 #035 ; (0x0023)        ; 036.016 0x24.0x10
      JEQ r00 r01 r02                ; 036.017 0x24.0x11
      LRC r01 #127 ; (0x007F)        ; 036.018 0x24.0x12
      LRC r02 #017 ; (0x0011)        ; 036.019 0x24.0x13
      ...
