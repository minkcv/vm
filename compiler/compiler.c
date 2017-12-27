#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAX_SYMBOLS 1024
#define MAX_ASSEMBLY_LINE_LENGTH 256
#define ASSEMBLY_START_LENGTH 256
#define SEGMENT_SIZE 256
#define CALLSTACK_SEGMENT 63
#define CALLSTACK_DEPTH_OFFSET 0
#define MAX_CODE_BLOCKS 1024
#define MAX_BLOCK_LABEL_LENGTH 256
#define SYMBOL_START_SEGMENT 59

enum BlockType
{
    Block_If = 0,
    Block_While = 1,
    Block_Function = 2,
};

enum SymbolType
{
    Symbol_Variable,
    Symbol_Constant,
};

typedef struct
{
    uint8_t segment;
    uint8_t offset;
    char* identifier;
    uint16_t value;
    enum SymbolType symbolType;
} Symbol;

typedef struct
{
    uint32_t labelId;
    enum BlockType blockType;

} BlockEnd;

Symbol* lookupSymbol(char* identifier, Symbol** map)
{
    int i = 0;
    Symbol* current = NULL;
    while (i < MAX_SYMBOLS)
    {
        current = map[i];
        if (current != NULL && !strcmp(current->identifier, identifier))
        {
            return current;
        }
        i++;
    }
    return NULL;
}

Symbol* addVariableToMap(char* identifier, Symbol** map)
{
    int i = 0; // Where the new symbol will go in the map
    int segment = SYMBOL_START_SEGMENT;
    int offset = 0;
    Symbol* lastVariable = NULL;
    while (i < MAX_SYMBOLS)
    {
        if (map[i] != NULL)
            if (map[i]->symbolType == Symbol_Variable)
                lastVariable = map[i];

        if (map[i] == NULL) // Previous symbol was the last
        {
            if (lastVariable != NULL)
            {
                // Use the next available segment/offset pair.
                segment = lastVariable->segment;
                offset = lastVariable->offset;
                if (offset == SEGMENT_SIZE - 1)
                {
                    segment++;
                    offset = 0;
                }
                else
                    offset++;

            }
            break;
        }
        i++;
    }
    Symbol* sym = malloc(sizeof(Symbol));
    sym->identifier = malloc(sizeof(char) * (strlen(identifier) + 1));
    strcpy(sym->identifier, identifier);
    sym->segment = segment;
    sym->offset = offset;
    sym->symbolType = Symbol_Variable;
    map[i] = sym;
    return sym;
}

Symbol* addConstantToMap(char* identifier, Symbol** map, uint16_t value)
{
    int i; // Where the new symbol will go in the map
    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        if (map[i] == NULL) // Previous symbol was the last
            break;
    }
    Symbol* sym = malloc(sizeof(Symbol));
    sym->identifier = malloc(sizeof(char) * (strlen(identifier) + 1));
    strcpy(sym->identifier, identifier);
    sym->value = value;
    sym->symbolType = Symbol_Constant;
    map[i] = sym;
    return sym;
}

void dumpSymbolMap(Symbol** map)
{
    int i = 0;
    Symbol* sym = NULL;
    printf("Symbol Map:\n");
    while (i < MAX_SYMBOLS)
    {
        sym = map[i];
        if (sym == NULL && i > 0)
            break;
        if (sym != NULL)
        {
            if (sym->symbolType == Symbol_Variable)
                printf("%s %d %d\n", sym->identifier, sym->segment, sym->offset);
            else if (sym->symbolType == Symbol_Constant)
                printf("%s %d\n", sym->identifier, sym->value);
        }
        i++;
    }
}

void functionReturn(char** assembly, uint32_t* currentAssemblyLine, uint16_t* instructionCount)
{
    // Load current callstack depth
    sprintf(assembly[*currentAssemblyLine], "LRC r0 #%d\n", CALLSTACK_SEGMENT);
    sprintf(assembly[*currentAssemblyLine + 1], "LRC r1 #%d\n", CALLSTACK_DEPTH_OFFSET);
    sprintf(assembly[*currentAssemblyLine + 2], "LDR r2 r0 r1\n");
    // Load the return offset
    sprintf(assembly[*currentAssemblyLine + 3], "LDR r3 r0 r2\n");
    // Load the return segment
    sprintf(assembly[*currentAssemblyLine + 4], "SUBC r2 #1\n");
    sprintf(assembly[*currentAssemblyLine + 5], "LDR r4 r0 r2\n");
    // Update the callstack depth (like pop)
    sprintf(assembly[*currentAssemblyLine + 6], "SUBC r2 #1\n");
    sprintf(assembly[*currentAssemblyLine + 7], "STR r2 r0 r1\n");
    // Jump to the return address
    sprintf(assembly[*currentAssemblyLine + 8], "JMP r4 r3\n");
    (*currentAssemblyLine) += 9;
    (*instructionCount) += 9;
}

// Returns the result of the expression in the register specified by "returnRegister"
// registers below "returnRegister" are considered to be in use by higher decompose calls
void decomposeExpression(char** expression, char** assembly, uint32_t* currentAssemblyLine, uint16_t* instructionCount, Symbol** map, uint8_t returnRegister, uint32_t sourceLine)
{
    if (returnRegister > 12)
    {
        printf("Expression on line %d is too complex and must be broken up\n", sourceLine);
        exit(1);
    }
    char* savePtr;
    char* token1 = strtok_r(*expression, " ", &savePtr);
    if (!strcmp(token1, "["))
    {
        // Memory access
        char* segmentExpression = strtok_r(NULL, ",", &savePtr);
        char* offsetExpression = strtok_r(NULL, "]", &savePtr);
        decomposeExpression(&segmentExpression, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
        decomposeExpression(&offsetExpression, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 2, sourceLine);
        sprintf(assembly[*currentAssemblyLine], "LDR r%d r%d r%d\n", returnRegister, returnRegister + 1, returnRegister + 2);
        (*currentAssemblyLine)++;
        (*instructionCount)++;
        return;
    }
    char* token2 = strtok_r(NULL, " ", &savePtr);
    char* token3 = strtok_r(NULL, "", &savePtr);

    if (token2 == NULL)
    {
        // The expression is an identifier, literal integer, or memory access
        Symbol* sym = lookupSymbol(token1, map);
        if (sym != NULL)
        {
            if (sym->symbolType == Symbol_Variable)
            {
                // Variable
                sprintf(assembly[*currentAssemblyLine], "LRC r%d #%d\n", returnRegister + 1, sym->segment);
                sprintf(assembly[*currentAssemblyLine + 1], "LRC r%d #%d\n", returnRegister + 2, sym->offset);
                sprintf(assembly[*currentAssemblyLine + 2], "LDR r%d r%d r%d\n", returnRegister, returnRegister + 1, returnRegister + 2);
                (*currentAssemblyLine) += 3;
                (*instructionCount) += 3;
            }
            else if (sym->symbolType == Symbol_Constant)
            {
                // Constant
                sprintf(assembly[*currentAssemblyLine], "LRC r%d #%d\n", returnRegister , sym->value);
                (*currentAssemblyLine)++;
                (*instructionCount)++;
            }
        }
        else
        {
            // Literal integer
            char* end;
            uint8_t literal = (uint8_t)strtol(token1, &end, 10);
            if ('\0' == end)
            {
                printf("Failed to parse %s as identifier or literal int\n", token1);
                exit(1);
            }
            else
            {
                sprintf(assembly[*currentAssemblyLine], "LRC r%d #%d\n", returnRegister, literal);
                (*currentAssemblyLine)++;
                (*instructionCount)++;
            }
        }
    }
    else if (token3 == NULL)
    {
        // If there are only 2 tokens in the expression, the first must be the operator
        if (!strcmp(token1, "~"))
        {
            Symbol* sym = lookupSymbol(token2, map);
            if (sym != NULL)
            {
                decomposeExpression(&token2, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
                sprintf(assembly[*currentAssemblyLine], "NOT r%d r%d\n", returnRegister, returnRegister + 1);
                (*currentAssemblyLine)++;
                (*instructionCount)++;
            }
        }
    }
    else
    {
        decomposeExpression(&token1, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
        decomposeExpression(&token3, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 2, sourceLine);
        if (!strcmp(token2, "+"))
        {
            sprintf(assembly[*currentAssemblyLine], "ADD r%d r%d r%d\n", returnRegister, returnRegister + 1, returnRegister + 2);
            (*currentAssemblyLine)++;
            (*instructionCount)++;
        }
        else if (!strcmp(token2, "-"))
        {
            sprintf(assembly[*currentAssemblyLine], "SUB r%d r%d r%d\n", returnRegister, returnRegister + 1, returnRegister + 2);
            (*currentAssemblyLine)++;
            (*instructionCount)++;
        }
        else if (!strcmp(token2, "&"))
        {
            sprintf(assembly[*currentAssemblyLine], "AND r%d r%d r%d\n", returnRegister, returnRegister + 1, returnRegister + 2);
            (*currentAssemblyLine)++;
            (*instructionCount)++;
        }
        else if (!strcmp(token2, "|"))
        {
            sprintf(assembly[*currentAssemblyLine], "OR r%d r%d r%d\n", returnRegister, returnRegister + 1, returnRegister + 2);
            (*currentAssemblyLine)++;
            (*instructionCount)++;
        }
        else if (!strcmp(token2, "^"))
        {
            sprintf(assembly[*currentAssemblyLine], "XOR r%d r%d r%d\n", returnRegister, returnRegister + 1, returnRegister + 2);
            (*currentAssemblyLine)++;
            (*instructionCount)++;
        }
        else if (!strcmp(token2, ">>"))
        {
            sprintf(assembly[*currentAssemblyLine], "LSR r%d r%d\n", returnRegister + 1, returnRegister + 2);
            sprintf(assembly[*currentAssemblyLine + 1], "CPY r%d r%d\n", returnRegister, returnRegister + 1);
            (*currentAssemblyLine) += 2;
            (*instructionCount) += 2;
        }
        else if (!strcmp(token2, "<<"))
        {
            sprintf(assembly[*currentAssemblyLine], "LSL r%d r%d\n", returnRegister + 1, returnRegister + 2);
            sprintf(assembly[*currentAssemblyLine + 1], "CPY r%d r%d\n", returnRegister, returnRegister + 1);
            (*currentAssemblyLine) += 2;
            (*instructionCount) += 2;
        }
        else if (!strcmp(token2, "=="))
        {
            sprintf(assembly[*currentAssemblyLine], "CMP r%d r%d r%d\n", returnRegister + 1, returnRegister + 1, returnRegister + 2);
            sprintf(assembly[*currentAssemblyLine + 1], "LRC r%d #1\n", returnRegister); // Start true (1)
            sprintf(assembly[*currentAssemblyLine + 2], "LRC r%d #%d\n", returnRegister + 2, (*instructionCount + 6) / SEGMENT_SIZE);
            sprintf(assembly[*currentAssemblyLine + 3], "LRC r%d #%d\n", returnRegister + 3, (*instructionCount + 6) % SEGMENT_SIZE);
            sprintf(assembly[*currentAssemblyLine + 4], "JEQ r%d r%d r%d\n", returnRegister + 1, returnRegister + 2, returnRegister + 3); // Jump over setting to false if equal
            sprintf(assembly[*currentAssemblyLine + 5], "LRC r%d #0\n", returnRegister); // Set false (0)
            (*currentAssemblyLine) += 6;
            (*instructionCount) += 6;
        }
        else if (!strcmp(token2, "<"))
        {
            sprintf(assembly[*currentAssemblyLine], "CMP r%d r%d r%d\n", returnRegister + 1, returnRegister + 1, returnRegister + 2);
            sprintf(assembly[*currentAssemblyLine + 1], "LRC r%d #1\n", returnRegister); // Start true (1)
            sprintf(assembly[*currentAssemblyLine + 2], "LRC r%d #%d\n", returnRegister + 2, (*instructionCount + 6) / SEGMENT_SIZE);
            sprintf(assembly[*currentAssemblyLine + 3], "LRC r%d #%d\n", returnRegister + 3, (*instructionCount + 6) % SEGMENT_SIZE);
            sprintf(assembly[*currentAssemblyLine + 4], "JLT r%d r%d r%d\n", returnRegister + 1, returnRegister + 2, returnRegister + 3); // Jump over setting to false if less than
            sprintf(assembly[*currentAssemblyLine + 5], "LRC r%d #0\n", returnRegister); // Set false (0)
            (*currentAssemblyLine) += 6;
            (*instructionCount) += 6;
        }
        else if (!strcmp(token2, ">"))
        {
            sprintf(assembly[*currentAssemblyLine], "CMP r%d r%d r%d\n", returnRegister + 1, returnRegister + 1, returnRegister + 2);
            sprintf(assembly[*currentAssemblyLine + 1], "LRC r%d #1\n", returnRegister); // Start true (1)
            sprintf(assembly[*currentAssemblyLine + 2], "LRC r%d #%d\n", returnRegister + 2, (*instructionCount + 6) / SEGMENT_SIZE);
            sprintf(assembly[*currentAssemblyLine + 3], "LRC r%d #%d\n", returnRegister + 3, (*instructionCount + 6) % SEGMENT_SIZE);
            sprintf(assembly[*currentAssemblyLine + 4], "JGT r%d r%d r%d\n", returnRegister + 1, returnRegister + 2, returnRegister + 3); // Jump over setting to false if greater than
            sprintf(assembly[*currentAssemblyLine + 5], "LRC r%d #0\n", returnRegister); // Set false (0)
            (*currentAssemblyLine) += 6;
            (*instructionCount) += 6;
        }
    }
}

uint32_t indexOfLastDot(char* string)
{
    uint32_t index = strlen(string);
    while (index > 0)
    {
        if (string[index] == '.')
            return index;

        index--;
    }
    return 0;
}

void allocAssembly(uint32_t currentSize, uint32_t newSize, char*** assembly)
{
    int i;
    char** newAssembly = malloc(sizeof(char*) * newSize);
    for (i = 0; i < newSize; i++)
    {
        newAssembly[i] = malloc(sizeof(char) * MAX_ASSEMBLY_LINE_LENGTH);
        memset(newAssembly[i], 0, MAX_ASSEMBLY_LINE_LENGTH);
    }
    if (*assembly != NULL)
    {
        for (i = 0; i < currentSize; i++)
        {
            strcpy(newAssembly[i], (*assembly)[i]);
            free((*assembly)[i]);
        }
    }
    if (*assembly != NULL)
        free(*assembly);

    *assembly = newAssembly;
}

int main (int argc, char** argv)
{
    char* filename = NULL;
    int i;
    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f"))
        {
            if (i + 1 < argc)
                filename = argv[i + 1];
        }
    }
    if (filename == NULL)
    {
        printf("Usage: compiler -f sourcefile.src\n");
        exit(1);
    }

    FILE* src = fopen(filename, "r");
    if (!src)
    {
         printf("Failed to open file %s\n", filename);
         exit(1);
    }

    uint32_t assemblySize = ASSEMBLY_START_LENGTH;
    char** assembly = NULL;
    allocAssembly(0, assemblySize, &assembly);
    char* lineIn = NULL;
    char* savePtr;
    size_t toRead = 0;
    ssize_t charsRead;
    int lineCount = 0;
    Symbol** symbolMap = malloc(sizeof(Symbol*) * MAX_SYMBOLS);
    uint16_t instructionCount = 0; // True binary instructions count
    uint32_t currentAssemblyLine = 0; // Number of lines in the assembly
    uint32_t endBlockLabelId = 1;
    int blockDepth = 0;
    BlockEnd** blockEnds = malloc(sizeof(BlockEnd*) * MAX_CODE_BLOCKS);
    for (i = 0; i < MAX_CODE_BLOCKS; i++)
    {
        blockEnds[i] = malloc(sizeof(BlockEnd));
        blockEnds[i]->blockType = 0;
        blockEnds[i]->labelId = 0;
    }

    while ((charsRead = getline(&lineIn, &toRead, src)) != -1)
    {
        if (currentAssemblyLine > assemblySize - 64)
        {
            // Getting close to the end of the assembly buffer. Reallocate it twice as large.
            allocAssembly(assemblySize, assemblySize * 2, &assembly);
            assemblySize = assemblySize * 2;
            if (instructionCount > 65536)
            {
                printf("Compiled program exceeds the maximum number of instructions\n");
                exit(1);
            }
        }
        lineCount++;
        if (lineIn[0] != '\n' )
        {
            char* token = strtok_r(lineIn, " ;\n", &savePtr);
            while (token != NULL)
            {
                if (strlen(token) > 1 && !strncmp(token, "//", 2))
                {
                    // First 2 chars of the token are '//'
                    // The rest of the line is a comment
                    break;
                }
                else if (!strcmp(token, "}"))
                {
                    blockDepth--;
                    if (blockDepth < 0)
                    {
                        printf("Missing open curly brace for close curly brace on line %d\n", lineCount);
                        exit(1);
                    }
                    if (blockEnds[blockDepth]->labelId > 0)
                    {
                        if (blockEnds[blockDepth]->blockType == Block_While)
                        {
                            // Ending a while block, jump to before the loop condition check
                            sprintf(assembly[currentAssemblyLine], "LRL r0 r1 @_while_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
                            sprintf(assembly[currentAssemblyLine + 1], "JMP r0 r1\n");
                            currentAssemblyLine += 2;
                            instructionCount += 3;
                        }
                        sprintf(assembly[currentAssemblyLine], "@_end_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
                        currentAssemblyLine++;
                    }
                    if (blockEnds[blockDepth]->blockType == Block_Function)
                    {
                        functionReturn(assembly, &currentAssemblyLine, &instructionCount);
                    }
                    blockEnds[blockDepth]->blockType = 0;
                    blockEnds[blockDepth]->labelId = 0;
                }
                else if (!strcmp(token, "while"))
                {
                    token = strtok_r(NULL, "{", &savePtr);
                    blockEnds[blockDepth]->labelId = endBlockLabelId;
                    blockEnds[blockDepth]->blockType = Block_While;

                    sprintf(assembly[currentAssemblyLine], "@_while_%d_%d\n", Block_While, endBlockLabelId);
                    currentAssemblyLine++;
                    // Load the result of the conditional expression into register 0
                    decomposeExpression(&token, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 0, lineCount);
                    // Compare the conditional expression with false (0)
                    sprintf(assembly[currentAssemblyLine], "LRC r1 #0\n");
                    sprintf(assembly[currentAssemblyLine + 1], "CMP r0 r0 r1\n");
                    sprintf(assembly[currentAssemblyLine + 2], "LRL r1 r2 @_end_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
                    // Jump past the while block if the conditional expression was false (0)
                    sprintf(assembly[currentAssemblyLine + 3], "JEQ r0 r1 r2\n");
                    currentAssemblyLine += 4;
                    instructionCount += 5;
                    endBlockLabelId++;
                    blockDepth++;
                }
                else if (!strcmp(token, "if"))
                {
                    token = strtok_r(NULL, "{", &savePtr);
                    blockEnds[blockDepth]->blockType = Block_If;
                    blockEnds[blockDepth]->labelId = endBlockLabelId;
                    endBlockLabelId++;
                    
                    // Load the result of the conditional expression into register 0
                    decomposeExpression(&token, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 0, lineCount);
                    // Compare the conditional expression with false (0)
                    sprintf(assembly[currentAssemblyLine], "LRC r1 #0\n");
                    sprintf(assembly[currentAssemblyLine + 1], "CMP r0 r0 r1\n");
                    sprintf(assembly[currentAssemblyLine + 2], "LRL r1 r2 @_end_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
                    // Jump past the if block if the conditional expression was false (0)
                    sprintf(assembly[currentAssemblyLine + 3], "JEQ r0 r1 r2\n");
                    currentAssemblyLine += 4;
                    instructionCount += 5;
                    blockDepth++;
                }
                else if (!strcmp(token, "var"))
                {
                    token = strtok_r(NULL, ";", &savePtr);
                    if (token == NULL)
                    {
                        printf("Expected identifier after 'var' on line %d\n", lineCount);
                        exit(1);
                    }
                    if (strstr(token, " "))
                    {
                        printf("Identifier cannon contain space on line %d\n", lineCount);
                        exit(1);
                    }
                    Symbol* sym = lookupSymbol(token, symbolMap);
                    if (sym != NULL)
                    {
                        printf("Redefinition of %s on line %d\n", token, lineCount);
                        exit(1);
                    }
                    addVariableToMap(token, symbolMap);
                }
                else if (!strcmp(token, "const"))
                {
                    token = strtok_r(NULL, " ", &savePtr);
                    if (token == NULL)
                    {
                        printf("Expected identifier after 'const' on line %d\n", lineCount);
                        exit(1);
                    }
                    Symbol* sym = lookupSymbol(token, symbolMap);
                    if (sym != NULL)
                    {
                        printf("Redefinition of %s on line %d\n", token, lineCount);
                        exit(1);
                    }
                    char* identifier = token;
                    token = strtok_r(NULL, ";", &savePtr);
                    char* end;
                    uint16_t value = (uint16_t)strtol(token, &end, 10);
                    if ('\0' ==  end)
                    {
                        printf("Failed to parse %s as identifier or literal int\n", token);
                        exit(1);
                    }
                    addConstantToMap(identifier, symbolMap, value);
                }
                else if (!strcmp(token, "func"))
                {
                    token = strtok_r(NULL, "{", &savePtr);
                    if (token == NULL)
                    {
                        printf("Expected identifier after '%s' on line %d\n", token, lineCount);
                        exit(1);
                    }
                    if (!strncmp(token, "_", 1))
                    {
                        printf("Function %s is not allowed to start with underbar on line %d\n", token, lineCount);
                        exit(1);
                    }
                    sprintf(assembly[currentAssemblyLine], "@%s\n", token);
                    currentAssemblyLine++;
                    blockEnds[blockDepth]->blockType = Block_Function;
                    blockDepth++;
                }
                else if (!strcmp(token, "return"))
                {
                    functionReturn(assembly, &currentAssemblyLine, &instructionCount);
                }
                else if (!strcmp(token, "call"))
                {
                    token = strtok_r(NULL, " ;", &savePtr);
                    uint16_t returnInstruction = instructionCount + 13;
                    // Load current callstack depth
                    sprintf(assembly[currentAssemblyLine], "LRC r0 #%d\n", CALLSTACK_SEGMENT);
                    sprintf(assembly[currentAssemblyLine + 1], "LRC r1 #%d\n", CALLSTACK_DEPTH_OFFSET);
                    sprintf(assembly[currentAssemblyLine + 2], "LDR r2 r0 r1\n");
                    // Increment the callstack depth by 1 for the return segment
                    sprintf(assembly[currentAssemblyLine + 3], "ADDC r2 #1\n");
                    // Store the return segment in the callstack
                    sprintf(assembly[currentAssemblyLine + 4], "LRC r3 #%d\n", returnInstruction / SEGMENT_SIZE);
                    sprintf(assembly[currentAssemblyLine + 5], "STR r3 r0 r2\n");
                    // Increment the callstack depth by 1 for the return offset
                    sprintf(assembly[currentAssemblyLine + 6], "ADDC r2 #1\n");
                    // Store the return offset in the callstack
                    sprintf(assembly[currentAssemblyLine + 7], "LRC r3 #%d\n", returnInstruction % SEGMENT_SIZE);
                    sprintf(assembly[currentAssemblyLine + 8], "STR r3 r0 r2\n");
                    // Store the callstack depth at 63.0
                    sprintf(assembly[currentAssemblyLine + 9], "STR r2 r0 r1\n");
                    // Load the function address and jump
                    sprintf(assembly[currentAssemblyLine + 10], "LRL r0 r1 @%s\n", token);
                    sprintf(assembly[currentAssemblyLine + 11], "JMP r0 r1\n");
                    currentAssemblyLine += 12;
                    instructionCount += 13;
                }
                else if (!strcmp(token, "halt"))
                {
                    sprintf(assembly[currentAssemblyLine], "HALT\n");
                    currentAssemblyLine++;
                    instructionCount++;
                }
                else if (strcmp(token, ";") && strcmp(token, "\n"))
                {
                    Symbol* sym = lookupSymbol(token, symbolMap);
                    if (sym != NULL)
                    {
                        token = strtok_r(NULL, " ", &savePtr);
                        if (!strcmp(token, "="))
                        {
                            token = strtok_r(NULL, ";", &savePtr);
                            decomposeExpression(&token, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 0, lineCount);

                            // Store the result (r0) into the right hand side identifier
                            sprintf(assembly[currentAssemblyLine], "LRC r1 #%d\n", sym->segment);
                            sprintf(assembly[currentAssemblyLine + 1], "LRC r2 #%d\n", sym->offset);
                            sprintf(assembly[currentAssemblyLine + 2], "STR r0 r1 r2\n");
                            currentAssemblyLine += 3;
                            instructionCount += 3;
                        }
                    }
                    else if (!strcmp(token, "["))
                    {
                        // Memory access
                        char* segmentExpression = strtok_r(NULL, ",", &savePtr);
                        char* offsetExpression = strtok_r(NULL, "]", &savePtr);
                        token = strtok_r(NULL, " ", &savePtr);
                        if (!strcmp(token, "="))
                        {
                            token = strtok_r(NULL, ";", &savePtr);
                            // Decompose the right hand side into r0
                            decomposeExpression(&token, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 0, lineCount);
                            // Decompose the segment and offset of the memory destination into r1 and r2
                            decomposeExpression(&segmentExpression, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 1, lineCount);
                            decomposeExpression(&offsetExpression, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 2, lineCount);
                            // Store the result into memory
                            sprintf(assembly[currentAssemblyLine], "STR r0 r1 r2\n");
                            currentAssemblyLine++;
                            instructionCount++;
                        }
                    }
                    else
                    {
                        printf("Unknown token %s on line %d\n", token, lineCount);
                        exit(1);
                    }
                }

                // Move to the next non space token
                token = strtok_r(NULL, " ", &savePtr);
            }

            free(lineIn);
            lineIn = NULL;
        }
    }
    fclose(src);
    printf("Read %d lines\n", lineCount);
    //dumpSymbolMap(symbolMap);
    uint32_t index = indexOfLastDot(filename);
    if (index == 0)
    {
        printf("Filename must have extension\n");
        exit(1);
    }
    char* noExtension = malloc(sizeof(char) * index);
    strncpy(noExtension, filename, index);
    noExtension[index] = '\0';
    FILE* asmfile = fopen(strcat(noExtension, ".asm"), "wb");
    for (i = 0; i < currentAssemblyLine; i++)
    {
        fwrite(assembly[i], sizeof(char), strlen(assembly[i]), asmfile);
    }
    fclose(asmfile);
    return 0;
}

