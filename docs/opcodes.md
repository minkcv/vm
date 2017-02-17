# Opcodes
The vm supports 16 different opcodes specified by the 4 most significant bits of each instruction. The opcodes are listed below by assembly instruction name and opcode in hexadecimal.

#### HALT 0x0
Causes the vm to stop executing instructions. Also closes the display and quits the entire program.

    HALT

#### ADD 0x1
Adds the values in the last first 2 registers and stores the value in the first register.

    ADD r1 r2 r3 ; r1 = r2 + r3

#### SUB 0x2
Subtracts the value in the third register from the value in the second register and stores the result in the first register. Keep in mind that the registers only store unsigned 8 bit values.

    SUB r1 r2 r3 ; r1 = r2 - r3

#### CMP 0x3
Compares the values in the second and third registers and stores the comparison result in the first register. 0 indicates the first register value was less. 1 indicates equality. 2 indicates the first register value was greater than the second.

    CMP r1 r2 r3

#### JLT 0x4
Jumps to the label if the value in the register is 0.

    JLT r1 @mylabel

#### JGT 0x5
Jumps to the label if the value in the register is 2.

    JGT r1 @mylabel

#### JEQ 0x6
Jumps to the label if the value in the register is 1.

    JEQ r1 @mylabel

#### JMP 0x7
Unconditionally jumps to the label. This is akin to a "goto".

    JMP @mylabel

#### CPY 0x8
Copies the value in the second register to the first register. Leaves the second register untouched. Sometimes referred to as "MOV" in other architectures.

    CPY r1 r2 ; r1 = r2

#### LDR 0x9
Loads the first register with the value in memory at the address specified by segment and offset where the value in the second register is the segment and the value in the third register is the offset.

    LDR r1 r2 r3 ; r1 = memory[r2][r3]

#### STR 0xA
Stores the value in the first register at the memory address specified by segment and offset where the value in the second register is the segment and the value in the third register is the offset.

    STR r1 r2 r3 ; memory[r2][r3] = r1

#### LRC 0xB
Loads the register with a constant value between decimal values 0 and 255 (inclusive). The constant can be specified with `#` for decimal or `$` for hexadecimal. Sometimes referred to as "load immediate" in other architectures.

    LRC r1 #103
    LRC r1 $67 ; same as above, just hex

#### AND 0xC
Binary ANDs the values in the second and third registers and stores the result in the first register.

    AND r1 r2 r3 ; r1 = r2 & r3

#### OR 0xD
Binary ORs the values in the second and third registers and stores the result in the first register.

    OR r1 r2 r3 ; r1 = r2 | r3

#### NOT 0xE
Binary NOTs the value in the second register and stores the result in the first register.

    NOT r1 r2 ; r1 = ~r2

#### SHF 0xF
Logical (bitwise) shifts the value in the first register by the amount specified by the value in the the second register in the direction specified by the value in the third register. Use 0 to shift left and 2 to shift right. Fills new places with zeroes. Stores the result in the first register.

    ; if (r3 == 0)
    ;   r1 << r2
    ; else if (r3 == 2)
    ;   r1 >> r2
    SHF r1 r2 r3
