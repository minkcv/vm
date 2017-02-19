# Opcodes
The vm supports 16 different opcodes specified by the 4 most significant bits of each instruction. The first opcode (0000) allows for 16 additional opcodes specified by the second 4 bits of each instruction. These additional opcodes only operate on two arguments but allow for more instructions to use 3 arguments.

Instructions that take register(s) as arguments can take any register r0 - r15 and can even take the same register for different arguments. There are no implicit registers like an accumulator or comparison register.

#### EXT 0x0
Indicates an extended opcode specified by the next 4 bits of the instruction.

##### EXT HALT 0x0
Causes the vm to stop executing instructions. Also closes the display and exits the program completely.

    ASM: HALT
    BIN: 0000 0000 0000 0000

##### EXT CPY 0x1
Copies the value in the second register to the first register. Leaves the second register untouched. Sometimes referred to as "MOV" in other architectures.

    ASM: CPY r3 r2 ; r3 = r2
    BIN: 0000 0001 0011 0010

##### EXT NOT 0x2
Binary NOTs the value in the second register and stores the result in the first register.

    ASM: NOT r3 r2 ; r3 = ~r2
    BIN: 0000 0010 0011 0010

##### EXT LSL 0x3
Logical shift lefts the value in the first register by the amount in the second register. Stores the value in the first register. Fills new bits with 0.

    ASM: LSL r3 r2
    BIN: 0000 0011 0011 0010

##### EXT LSR 0x4
Logical shift rights the value in the first register by the amount in the second register. Stores the value in the first register. Fills new bits with 0.

    ASM: LSR r3 r2
    BIN: 0000 0100 0011 0010

##### EXT JMP 0x5
Jumps to the address specified by the values in the registers where the first register holds the segment address and the second register holds the offset.

    ASM: JMP r3 r2
    BIN: 0000 0101 0011 0010

#### ADD 0x1
Adds the values in the last 2 registers and stores the value in the first register.

    ASM: ADD r3 r1 r2 ; r3 = r1 + r2
    BIN: 0001 0011 0001 0010

#### SUB 0x2
Subtracts the value in the third register from the value in the second register and stores the result in the first register. Keep in mind that the registers only store unsigned 8 bit values.

    ASM: SUB r1 r2 r3 ; r1 = r2 - r3
    BIN: 0010 0001 0010 0011

#### ADDC 0x3
Adds the constant value to the register.

    ASM: ADDC r1 #123
    BIN: 0011 0001 0111 1011

#### SUBC 0x4
Subtracts the constant value from the register.

    ASM: SUBC r1 #123
    BIN: 0100 0001 0111 1011

#### CMP 0x5
Compares the values in the second and third registers and stores the comparison result in the first register. 0 indicates the first register value was less. 1 indicates equality. 2 indicates the first register value was greater than the second.

    ASM: CMP r5 r4 r3
    BIN: 0101 0101 0100 0011

#### JLT 0x6
Jumps to the address specified by the second and third registers if the value in the first register is 0.

    ASM: JLT r1 r4 r3
    BIN: 0110 0001 0100 0011

#### JGT 0x7
Jumps to the address specified by the second and third registers if the value in the first register is 2.

    ASM: JGT r1 r4 r3
    BIN: 0111 0001 0100 0011

#### JEQ 0x8
Jumps to the address specified by the second and third registers if the value in the first register is 1.

    ASM: JEQ r1 r4 r3
    BIN: 1000 0001 0100 0011

#### LDR 0x9
Loads the first register with the value in memory at the address specified by segment and offset where the value in the second register is the segment and the value in the third register is the offset.

    ASM: LDR r1 r4 r3 ; r1 = memory[r4][r3]
    BIN: 1001 0001 0100 0011

#### STR 0xA
Stores the value in the first register at the memory address specified by segment and offset where the value in the second register is the segment and the value in the third register is the offset.

    ASM: STR r1 r4 r3 ; memory[r4][r3] = r1
    BIN: 1010 0001 0100 0011

#### LRC 0xB
Loads the register with a constant value between decimal values 0 and 255 (inclusive). Sometimes referred to as "load immediate" in other architectures.

    ASM: LRC r1 #103
    BIN: 1011 0001 0110 0111

#### AND 0xC
Binary ANDs the values in the second and third registers and stores the result in the first register.

    ASM: AND r1 r4 r3 ; r1 = r4 & r3
    BIN: 1100 0001 0100 0011

#### OR 0xD
Binary ORs the values in the second and third registers and stores the result in the first register.

    ASM: OR r1 r4 r3 ; r1 = r4 | r3
    BIN: 1101 0001 0100 0011
