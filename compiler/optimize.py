#!/usr/bin/python3
import sys
import os

asmName = ''
if len(sys.argv) > 2 and sys.argv[1] == '-f' and len(sys.argv[2]) > 0:
    asmName = sys.argv[2]
else:
    print("Usage: optimize.py -f assembly.asm")
    exit()

asmFile = open(asmName, 'r')
dotIndex = asmName.rfind('.')
if dotIndex < 0:
    print("Input file should have .asm extension")
    exit()
newName = asmName[:dotIndex] + '.opt.asm'

optimized = []
def invalidateRegs():
    global regs
    global remapRegs
    global previousRegs
    regs = [-1] * 16
    remapRegs = [-1] * 16
    previousRegs = [-1] * 16

def correctLine(arg):
    global remapRegs
    global tokens
    global corrected
    global correctedLine
    if remapRegs[int(tokens[arg][1:])] > -1 and remapRegs[int(tokens[arg][1:])] != int(tokens[arg][1:]):
        corrected = True
        correctedLine += 'r' + str(remapRegs[int(tokens[arg][1:])]) + ' '
    else:
        correctedLine += tokens[arg] + ' '

def correctJump():
    global lineIndex
    global opcount
    global optimized
    line1 = optimized[-2]
    line2 = optimized[-1]
    tokens1 = line1.split(' ')
    tokens2 = line2.split(' ')
    if tokens1[0] == 'LRC' and tokens2[0] == 'LRC':
        segment = int((opcount + 2) / 256)
        offset = (opcount + 2) % 256
        line1 = line1[:line1.find('#') + 1] + str(segment)
        line2 = line2[:line2.find('#') + 1] + str(offset)
        optimized[-2] = line1
        optimized[-1] = line2
        print('corrected jump')

def correctCallJump():
    line1 = optimized[-6]
    line2 = optimized[-3]
    tokens1 = line1.split(' ')
    tokens2 = line2.split(' ')
    if tokens1[0] == 'LRC' and tokens2[0] == 'LRC':
        segment = int((opcount + 3) / 256)
        offset = (opcount + 3) % 256
        line1 = line1[:line1.find('#') + 1] + str(segment)
        line2 = line2[:line2.find('#') + 1] + str(offset)
        optimized[-6] = line1
        optimized[-3] = line2
        print('corrected return address')

def dirtyRemap():
    global remapRegs
    global arg1
    remapRegs[arg1] = -1
    try:
        idx = remapRegs.index(arg1)
        remapRegs[idx] = -1
    except ValueError:
        pass

regs = [-1] * 16
previousRegs = [0] * 16
remapRegs = [-1] * 16
lastJump = 0
nextJump = 0
lineIndex = 0
opsaved = 0

opcount = 0
originalopcount = 0
original = []
for line in asmFile:
    original.append(line)

for line in original:
    print(f'Line {lineIndex}')
    lineIndex += 1
    line = line.translate(str.maketrans('', '', '\n'))
    tokens = line.split(' ')
    corrected = False
    correctedLine = tokens[0] + ' '
    skipLine = False
    if line[0] == '@':
        optimized.append(line)
        invalidateRegs()
        continue

    originalopcount += 1
    arg1 = -1
    if len(tokens) > 1:
        arg1 = int(tokens[1][1:])
    arg2 = -1
    arg3 = -1
    if len(tokens) > 2:
        arg2 = int(tokens[2][1:])
    if len(tokens) > 3:
        if tokens[3][0] != '@':
            arg3 = int(tokens[3][1:])

    if tokens[0] == 'JMP' or tokens[0] == 'JLT' or tokens[0] == 'JGT' or tokens[0] == 'JEQ':
        lastJump = opcount
    
    for i in range(lineIndex, len(original)):
        nextTokens = original[i].split(' ')
        if nextTokens[0] == 'JMP' or nextTokens[0] == 'JLT' or nextTokens[0] == 'JGT' or nextTokens[0] == 'JEQ':
            nextJump = opcount + (i - lineIndex)
            break

    if tokens[0] == 'LRC':
        # arg2 is a constant in this case
        remapRegs[arg1] = -1
        if opcount - lastJump > 3 and nextJump - opcount > 9:
            if previousRegs[arg1] == arg2:
                print(f'reg {arg1} already has value {arg2}')
                skipLine = True
            elif arg1 in remapRegs:
                newReg = -1
                print(f'regs {regs}')
                print(f'remap {remapRegs}')
                for i in range(arg1 + 1, 16):
                    if regs[i] == -1 and remapRegs[i] == -1:
                        newReg = i
                        break
                remapRegs[arg1] = newReg
                print(f'used {newReg} in place of {arg1}')
                skipLine = True
                optimized.append(f'LRC r{newReg} #{arg2}')
                opcount += 1
            else:
                try:
                    reg = previousRegs.index(arg2)
                    print(f'regs {regs}')
                    print(f'reg {reg} already has value {arg2}')
                    skipLine = True
                    remapRegs[arg1] = reg
                except ValueError:
                    pass
        if not skipLine:
            regs[arg1] = arg2
        
    elif tokens[0] == 'CPY':
        correctedLine += tokens[1] + ' '
        correctLine(2)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'NOT':
        regs[arg1] = -1
        correctedLine += tokens[1] + ' '
        correctLine(2)
        dirtyRemap()
    elif tokens[0] == 'LSL':
        regs[arg1] = -1
        correctedLine += tokens[1] + ' '
        correctLine(2)
        dirtyRemap()
    elif tokens[0] == 'LSR':
        regs[arg1] = -1
        correctedLine += tokens[1] + ' '
        correctLine(2)
        dirtyRemap()
    elif tokens[0] == 'JMP':
        correctJump()
        invalidateRegs()
    elif tokens[0] == 'ADD':
        correctedLine += tokens[1] + ' '
        correctLine(2)
        correctLine(3)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'SUB':
        correctLine(2)
        correctLine(3)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'ADDC':
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'SUBC':
        regs[arg1] = -1
    elif tokens[0] == 'CMP':
        correctedLine += tokens[1] + ' '
        correctLine(2)
        correctLine(3)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'JLT':
        correctJump()
        correctedLine += tokens[1] + ' '
        invalidateRegs()
        correctLine(2)
        correctLine(3)
    elif tokens[0] == 'JGT':
        correctJump()
        correctedLine += tokens[1] + ' '
        invalidateRegs()
        correctLine(2)
        correctLine(3)
    elif tokens[0] == 'JEQ':
        correctJump()
        correctedLine += tokens[1] + ' '
        invalidateRegs()
        correctLine(2)
        correctLine(3)
    elif tokens[0] == 'LDR':
        correctedLine += tokens[1] + ' '
        correctLine(2)
        correctLine(3)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'STR':
        correctLine(1)
        correctLine(2)
        correctLine(3)
    elif tokens[0] == 'AND':
        correctedLine += tokens[1] + ' '
        correctLine(2)
        correctLine(3)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'OR':
        correctedLine += tokens[1] + ' '
        correctLine(2)
        correctLine(3)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'XOR':
        correctedLine += tokens[1] + ' '
        correctLine(2)
        correctLine(3)
        dirtyRemap()
        regs[arg1] = -1
    elif tokens[0] == 'LRL':
        originalopcount += 1
        if tokens[3][1] != '_':
            correctCallJump()
    
    if not skipLine:
        if corrected:
            optimized.append(correctedLine + '; ' + line)
        else:
            optimized.append(line)
        opcount += 1
        if tokens[0] == 'LRL':
            opcount += 1
    else:
        opsaved += 1
        optimized.append('; ' + line)
        
    previousRegs = regs.copy()

optimizedStr = """{}
""".format("\n".join(optimized))
out = open(newName, 'w')
out.write(optimizedStr)
asmFile.close()
out.close()
print(f'Saved {opsaved} ({originalopcount - opcount}) instructions ({originalopcount} to {opcount})')