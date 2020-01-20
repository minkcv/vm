#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#define MAX_SYMBOLS 1024
#define MAX_ASSEMBLY_LINE_LENGTH 256
#define ASSEMBLY_START_LENGTH 256
#define SEGMENT_SIZE 256
#define CALLSTACK_SEGMENT 63
#define CALLSTACK_DEPTH_OFFSET 0
#define MAX_CODE_BLOCKS 1024
#define MAX_BLOCK_LABEL_LENGTH 256
#define SYMBOL_START_SEGMENT 59

int logSourceCode = 0;
int logInstructionAddress = 0;

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

void sprintfAsm(char** assemblyBuffer, uint32_t* currentAssemblyLine, int32_t instructionCount, char* format, ...)
{
    int commentColumnStart = 30;
    va_list args;
    va_start(args, format);
    int nWritten = vsnprintf(assemblyBuffer[*currentAssemblyLine], MAX_ASSEMBLY_LINE_LENGTH, format, args);
    if (nWritten > commentColumnStart)
        commentColumnStart = nWritten + 5;
    va_end(args);

    if (logInstructionAddress && instructionCount >= 0)
    {
        // Pad spaces after the instruction so comments are in a column
        int i;
        for (i = nWritten; i <= commentColumnStart; i++)
        {
            *(assemblyBuffer[*currentAssemblyLine] + i - 1) = ' ';
        }
        // Write the instruction address in decimal and hex
        sprintf(assemblyBuffer[*currentAssemblyLine] + commentColumnStart, 
                " ; %03d.%03d  0x%02X.0x%02X\n", 
                instructionCount / SEGMENT_SIZE, instructionCount % SEGMENT_SIZE,
                instructionCount / SEGMENT_SIZE, instructionCount % SEGMENT_SIZE);
    }
    
    (*currentAssemblyLine)++;
}

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

void functionReturn(char** assembly, uint32_t* currentAssemblyLine, int32_t* instructionCount)
{
    // Load current callstack depth
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r0 #%d\n", CALLSTACK_SEGMENT);
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r1 #%d\n", CALLSTACK_DEPTH_OFFSET);
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LDR r2 r0 r1\n");
    // Load the return offset
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LDR r3 r0 r2\n");
    // Load the return segment
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "SUBC r2 #1\n");
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LDR r4 r0 r2\n");
    // Update the callstack depth (like pop)
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "SUBC r2 #1\n");
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "STR r2 r0 r1\n");
    // Jump to the return address
    sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "JMP r4 r3\n");
}

// Returns the result of the expression in the register specified by "returnRegister"
// registers below "returnRegister" are considered to be in use by higher decompose calls
void decomposeExpression(char** expression, char** assembly, uint32_t* currentAssemblyLine, int32_t* instructionCount, Symbol** map, uint8_t returnRegister, uint32_t sourceLine)
{
    if (returnRegister > 12)
    {
        printf("Expression on line %d is too complex and must be broken up\n", sourceLine);
        exit(1);
    }
    char* savePtr;
    // If the expression contains any comparisons then separate them and decompose.
    if (strstr(*expression, " == "))
    {
        int stopIndex = strstr(*expression, "==") - *expression;
        char* after = (*expression) + stopIndex + 3;
        (*expression)[stopIndex] = '\0';
        (*expression)[stopIndex + 1] = '\0';
        decomposeExpression(expression, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
        decomposeExpression(&after, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 2, sourceLine);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "CMP r%d r%d r%d\n", returnRegister + 1, returnRegister + 1, returnRegister + 2);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #1\n", returnRegister); // Start true (1)
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister + 2, (*instructionCount + 4) / SEGMENT_SIZE);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister + 3, (*instructionCount + 3) % SEGMENT_SIZE);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "JEQ r%d r%d r%d\n", returnRegister + 1, returnRegister + 2, returnRegister + 3); // Jump over setting to false if equal
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #0\n", returnRegister); // Set false (0)
        return;
    }
    if (strstr(*expression, " < "))
    {
        int stopIndex = strstr(*expression, " < ") - *expression;
        char* after = (*expression) + stopIndex + 3;
        (*expression)[stopIndex + 1] = '\0';
        decomposeExpression(expression, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
        decomposeExpression(&after, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 2, sourceLine);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "CMP r%d r%d r%d\n", returnRegister + 1, returnRegister + 1, returnRegister + 2);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #1\n", returnRegister); // Start true (1)
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister + 2, (*instructionCount + 4) / SEGMENT_SIZE);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister + 3, (*instructionCount + 3) % SEGMENT_SIZE);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "JLT r%d r%d r%d\n", returnRegister + 1, returnRegister + 2, returnRegister + 3); // Jump over setting to false if less than
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #0\n", returnRegister); // Set false (0)
        return;
    }
    if (strstr(*expression, " > "))
    {
        int stopIndex = strstr(*expression, " > ") - *expression;
        char* after = (*expression) + stopIndex + 3;
        (*expression)[stopIndex + 1] = '\0';
        decomposeExpression(expression, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
        decomposeExpression(&after, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 2, sourceLine);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "CMP r%d r%d r%d\n", returnRegister + 1, returnRegister + 1, returnRegister + 2);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #1\n", returnRegister); // Start true (1)
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister + 2, (*instructionCount + 4) / SEGMENT_SIZE);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister + 3, (*instructionCount + 3) % SEGMENT_SIZE);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "JGT r%d r%d r%d\n", returnRegister + 1, returnRegister + 2, returnRegister + 3); // Jump over setting to false if greater than
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #0\n", returnRegister); // Set false (0)
        return;
    }
    char* token1 = strtok_r(*expression, " \n;", &savePtr);
    if (!strcmp(token1, "["))
    {
        // Memory access
        char* segmentExpression = strtok_r(NULL, ",", &savePtr);
        char* offsetExpression = strtok_r(NULL, "]", &savePtr);
        decomposeExpression(&segmentExpression, assembly, currentAssemblyLine, instructionCount, map, returnRegister, sourceLine);
        decomposeExpression(&offsetExpression, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
        sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LDR r%d r%d r%d\n", returnRegister, returnRegister, returnRegister + 1);
        return;
    }

    // Now try to parse symbols, literals, and operators.
    char* token2 = strtok_r(NULL, " \n;", &savePtr);
    char* token3 = strtok_r(NULL, "", &savePtr);

    if (token2 == NULL)
    {
        // The expression is a symbol or literal integer
        Symbol* sym = lookupSymbol(token1, map);
        if (sym != NULL)
        {
            if (sym->symbolType == Symbol_Variable)
            {
                // Variable
                sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister, sym->segment);
                sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister + 1, sym->offset);
                sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LDR r%d r%d r%d\n", returnRegister, returnRegister, returnRegister + 1);
            }
            else if (sym->symbolType == Symbol_Constant)
            {
                // Constant
                sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister , sym->value);
            }
        }
        else
        {
            // Literal integer
            char* end;
            uint8_t literal = (uint8_t)strtol(token1, &end, 10);
            if (end != token1)
            {
                sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LRC r%d #%d\n", returnRegister, literal);
            }
            else
            {
                printf("Failed to parse %s as identifier or literal int\n", token1);
                exit(1);
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
                decomposeExpression(&token2, assembly, currentAssemblyLine, instructionCount, map, returnRegister, sourceLine);
                sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "NOT r%d r%d\n", returnRegister, returnRegister);
            }
        }
        else
        {
            printf("Unknown expression on line %d\n", sourceLine);
            exit(1);
        }
    }
    else
    {
        decomposeExpression(&token1, assembly, currentAssemblyLine, instructionCount, map, returnRegister, sourceLine);
        decomposeExpression(&token3, assembly, currentAssemblyLine, instructionCount, map, returnRegister + 1, sourceLine);
        if (!strcmp(token2, "+"))
        {
            sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "ADD r%d r%d r%d\n", returnRegister, returnRegister, returnRegister + 1);
        }
        else if (!strcmp(token2, "-"))
        {
            sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "SUB r%d r%d r%d\n", returnRegister, returnRegister, returnRegister + 1);
        }
        else if (!strcmp(token2, "&"))
        {
            sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "AND r%d r%d r%d\n", returnRegister, returnRegister, returnRegister + 1);
        }
        else if (!strcmp(token2, "|"))
        {
            sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "OR r%d r%d r%d\n", returnRegister, returnRegister, returnRegister + 1);
        }
        else if (!strcmp(token2, "^"))
        {
            sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "XOR r%d r%d r%d\n", returnRegister, returnRegister, returnRegister + 1);
        }
        else if (!strcmp(token2, ">>"))
        {
            sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LSR r%d r%d\n", returnRegister, returnRegister + 1);
        }
        else if (!strcmp(token2, "<<"))
        {
            sprintfAsm(assembly, currentAssemblyLine, (*instructionCount)++, "LSL r%d r%d\n", returnRegister, returnRegister + 1);
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
    char* logOptions = NULL;
    int i;
    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f"))
        {
            if (i + 1 < argc)
                filename = argv[i + 1];
        }
        if (!strcmp(argv[i], "-l"))
        {
            if (i + 1 < argc)
            {
                logOptions = argv[i + 1];
            }
            else
            {
                printf("Logging options usage: -l [options]\n");
                printf("Options:\n");
                printf("\ts: source code\n");
                printf("\ta: instruction addresses\n");
                exit(1);
            }
        }
    }
    if (filename == NULL)
    {
        printf("Usage: compiler -f sourcefile.src\n");
        exit(1);
    }
    if (logOptions != NULL)
    {
        int nOptions = strlen(logOptions);
        int i;
        for (i = 0; i < nOptions; i++)
        {
            if (logOptions[i] == 's')
                logSourceCode = 1;
            if (logOptions[i] == 'a')
                logInstructionAddress = 1;
        }
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
    int32_t instructionCount = 0; // True binary instructions count
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
            if (logSourceCode)
            {
                sprintfAsm(assembly, &currentAssemblyLine, -1, "; %s", lineIn);
            }
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
                            sprintfAsm(assembly, &currentAssemblyLine, instructionCount, "LRL r0 r1 @_while_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
                            instructionCount += 2;
                            sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "JMP r0 r1\n");
                        }
                        sprintfAsm(assembly, &currentAssemblyLine, -1, "@_end_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
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

                    sprintfAsm(assembly, &currentAssemblyLine, -1, "@_while_%d_%d\n", Block_While, endBlockLabelId);
                    // Load the result of the conditional expression into register 0
                    decomposeExpression(&token, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 0, lineCount);
                    // Compare the conditional expression with false (0)
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r1 #0\n");
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "CMP r0 r0 r1\n");
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount, "LRL r1 r2 @_end_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
                    instructionCount += 2;
                    // Jump past the while block if the conditional expression was false (0)
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "JEQ r0 r1 r2\n");
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
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r1 #0\n");
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "CMP r0 r0 r1\n");
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount, "LRL r1 r2 @_end_%d_%d\n", blockEnds[blockDepth]->blockType, blockEnds[blockDepth]->labelId);
                    instructionCount += 2;
                    // Jump past the if block if the conditional expression was false (0)
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "JEQ r0 r1 r2\n");
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
                    if (NULL == end)
                    {
                        printf("Failed to parse %s as identifier or literal int\n", token);
                        exit(1);
                    }
                    addConstantToMap(identifier, symbolMap, value);
                }
                else if (!strcmp(token, "func"))
                {
                    token = strtok_r(NULL, "{", &savePtr);
                    // Trim trailing space after function name and before {
                    char* firstSpace = strstr(token, " ");
                    if (firstSpace != NULL) 
                        strncpy(firstSpace, "\0", 1);
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
                    sprintfAsm(assembly, &currentAssemblyLine, -1, "@%s\n", token);
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
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r0 #%d\n", CALLSTACK_SEGMENT);
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r1 #%d\n", CALLSTACK_DEPTH_OFFSET);
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LDR r2 r0 r1\n");
                    // Increment the callstack depth by 1 for the return segment
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "ADDC r2 #1\n");
                    // Store the return segment in the callstack
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r3 #%d\n", returnInstruction / SEGMENT_SIZE);
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "STR r3 r0 r2\n");
                    // Increment the callstack depth by 1 for the return offset
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "ADDC r2 #1\n");
                    // Store the return offset in the callstack
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r3 #%d\n", returnInstruction % SEGMENT_SIZE);
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "STR r3 r0 r2\n");
                    // Store the callstack depth at 63.0
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "STR r2 r0 r1\n");
                    // Load the function address and jump
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount, "LRL r0 r1 @%s\n", token);
                    instructionCount += 2;
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "JMP r0 r1\n");
                }
                else if (!strcmp(token, "halt"))
                {
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "HALT\n");
                }
                else if (!strcmp(token, "break"))
                {
                    sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "BRKP\n");
                }
                else if (strcmp(token, ";") && strcmp(token, "\n"))
                {
                    Symbol* sym = lookupSymbol(token, symbolMap);
                    if (sym != NULL)
                    {
                        token = strtok_r(NULL, " ", &savePtr);
                        if (!strcmp(token, "="))
                        {
                            token = strtok_r(NULL, "\n;", &savePtr);
                            decomposeExpression(&token, assembly, &currentAssemblyLine, &instructionCount, symbolMap, 0, lineCount);

                            // Store the result (r0) into the right hand side identifier
                            sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r1 #%d\n", sym->segment);
                            sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "LRC r2 #%d\n", sym->offset);
                            sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "STR r0 r1 r2\n");
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
                            sprintfAsm(assembly, &currentAssemblyLine, instructionCount++, "STR r0 r1 r2\n");
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
    // Add space for 5 new chars ".asm\0"
    char* noExtension = malloc(sizeof(char) * index + sizeof(char) * 5);
    strncpy(noExtension, filename, index);
    noExtension[index] = '\0';
    FILE* asmfile = fopen(strcat(noExtension, ".asm"), "wb");
    if (asmfile == NULL)
    {
        printf("Couldn't open file %s for writing assembly to\n", noExtension);
        exit(1);
    }
    for (i = 0; i < currentAssemblyLine; i++)
    {
        fwrite(assembly[i], sizeof(char), strlen(assembly[i]), asmfile);
    }
    printf("Wrote %d lines (%d instructions) to %s\n", currentAssemblyLine, instructionCount, noExtension);
    fclose(asmfile);
    return 0;
}

