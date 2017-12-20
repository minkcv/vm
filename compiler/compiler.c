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

typedef struct
{
    uint8_t segment;
    uint8_t offset;
    char* identifier;
} Symbol;

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

Symbol* addSymbolToMap(char* identifier, Symbol*** map)
{
    int i = 0; // Where the new symbol will go in the map
    int segment = 0;
    int offset = 0;
    Symbol* existing = NULL;
    while (i < MAX_SYMBOLS)
    {
        if ((*map)[0] == NULL) // No symbols yet
            break;

        if ((*map)[i] == NULL) // Previous symbol was the last
        {
            // Use the next available segment/offset pair.
            segment = existing->segment;
            offset = existing->offset;
            if (offset == 255)
            {
                segment++;
                offset = 0;
            }
            else
                offset++;

            break;
        }
        existing = (*map)[i];
        i++;
    }
    Symbol* sym = malloc(sizeof(Symbol));
    sym->identifier = malloc(sizeof(char) * (strlen(identifier) + 1));
    strcpy(sym->identifier, identifier);
    sym->segment = segment;
    sym->offset = offset;
    (*map)[i] = sym;
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
            printf("%s %d %d\n", sym->identifier, sym->segment, sym->offset);
        }
        i++;
    }
}

// Returns the number of instructions added to the assembly.
// Returns the result of the expression in register r0
void decomposeExpression(char** expression, char*** assembly, uint32_t* currentAssemblyLine, Symbol** map)
{
    char* savePtr;
    char* token1 = strtok_r(*expression, " ", &savePtr);
    char* token2 = strtok_r(NULL, " ", &savePtr);
    char* token3 = strtok_r(NULL, " ", &savePtr);

    if (token2 == NULL)
    {
        // The expression is just an identifier or literal integer
        Symbol* sym = lookupSymbol(token1, map);
        if (sym != NULL)
        {
            // Identifier
            sprintf((*assembly)[*currentAssemblyLine], "LRC r1 #%d\n", sym->segment);
            sprintf((*assembly)[*currentAssemblyLine + 1], "LRC r2 #%d\n", sym->offset);
            sprintf((*assembly)[*currentAssemblyLine + 2], "LDR r0 r1 r2\n");
            (*currentAssemblyLine) += 3;
        }
        else
        {
            // Literal integer
            char* end;
            uint8_t literal = (uint8_t)strtol(token1, &end, 10);
            if (!strcmp(token1, end))
            {
                printf("Failed to parse %s as identifier or literal int\n", token1);
                exit(1);
            }
            else
            {
                sprintf((*assembly)[*currentAssemblyLine], "LRC r0 #%d\n", literal);
                (*currentAssemblyLine)++;
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
                decomposeExpression(&token2, assembly, currentAssemblyLine, map);
                sprintf((*assembly)[*currentAssemblyLine], "NOT r0 r0\n");
            }
        }
    }
    else
    {
        decomposeExpression(&token1, assembly, currentAssemblyLine, map);
        sprintf((*assembly)[*currentAssemblyLine], "CPY r15 r0\n");
        (*currentAssemblyLine)++;
        decomposeExpression(&token3, assembly, currentAssemblyLine, map);
        if (!strcmp(token2, "+"))
        {
            sprintf((*assembly)[*currentAssemblyLine], "ADD r0 r0 r15\n");
        }
        else if (!strcmp(token2, "-"))
        {
            sprintf((*assembly)[*currentAssemblyLine], "SUB r0 r0 r15\n");
        }
        else if (!strcmp(token2, "&"))
        {
            sprintf((*assembly)[*currentAssemblyLine], "AND r0 r0 r15\n");
        }
        else if (!strcmp(token2, "|"))
        {
            sprintf((*assembly)[*currentAssemblyLine], "OR r0 r0 r15\n");
        }
        else if (!strcmp(token2, "^"))
        {
            sprintf((*assembly)[*currentAssemblyLine], "XOR r0 r0 r15\n");
        }
        else if (!strcmp(token2, ">>"))
        {
        }
        else if (!strcmp(token2, "<<"))
        {
        }
        else if (!strcmp(token2, "=="))
        {
        }
        else if (!strcmp(token2, "<"))
        {
        }
        else if (!strcmp(token2, ">"))
        {
        }
        (*currentAssemblyLine)++;
    }
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

    char** assembly = malloc(sizeof(char*) * ASSEMBLY_START_LENGTH);
    for (i = 0; i < ASSEMBLY_START_LENGTH; i++)
    {
        assembly[i] = malloc(sizeof(char) * MAX_ASSEMBLY_LINE_LENGTH);
        memset(assembly[i], 0, MAX_ASSEMBLY_LINE_LENGTH);
    }
    char* lineIn = NULL;
    char* savePtr;
    size_t toRead = 0;
    ssize_t charsRead;
    int lineCount = 0;
    uint32_t numInstructions = 0;
    Symbol** symbolMap = malloc(sizeof(Symbol*) * MAX_SYMBOLS);
    uint32_t currentAssemblyLine = 0;

    while ((charsRead = getline(&lineIn, &toRead, src)) != -1)
    {
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
                else if (!strcmp(token, "var"))
                {
                    token = strtok_r(NULL, " ;", &savePtr);
                    if (token == NULL)
                    {
                        printf("Expected identifier after '%s' on line %d\n", token, lineCount);
                        exit(1);
                    }
                    Symbol* sym = lookupSymbol(token, symbolMap);
                    if (sym != NULL)
                    {
                        printf("Redefinition of %s on line %d\n", token, lineCount);
                        exit(1);
                    }
                    addSymbolToMap(token, &symbolMap);
                }
                else if (!strcmp(token, "func"))
                {
                    token = strtok_r(NULL, " {", &savePtr);
                    if (token == NULL)
                    {
                        printf("Expected identifier after '%s' on line %d\n", token, lineCount);
                        exit(1);
                    }
                    if (lookupSymbol(token, symbolMap) != NULL)
                    {
                        printf("Redefinition of %s on line %d\n", token, lineCount);
                        exit(1);
                    }
                    addSymbolToMap(token, &symbolMap);
                    sprintf(assembly[currentAssemblyLine], "@%s\n", token);
                    currentAssemblyLine++;
                }
                else if (!strcmp(token, "return"))
                {
                    // Load current callstack depth
                    sprintf(assembly[currentAssemblyLine], "LRC r0 #%d\n", CALLSTACK_SEGMENT);
                    sprintf(assembly[currentAssemblyLine + 1], "LRC r1 #%d\n", CALLSTACK_DEPTH_OFFSET);
                    sprintf(assembly[currentAssemblyLine + 2], "LDR r2 r0 r1\n");
                    // Load the return offset
                    sprintf(assembly[currentAssemblyLine + 3], "LDR r3 r0 r2\n");
                    // Load the return segment
                    sprintf(assembly[currentAssemblyLine + 4], "SUBC r2 #1\n");
                    sprintf(assembly[currentAssemblyLine + 5], "LDR r4 r0 r2\n");
                    // Update the callstack depth (like pop)
                    sprintf(assembly[currentAssemblyLine + 6], "SUBC r2 #1\n");
                    sprintf(assembly[currentAssemblyLine + 7], "STR r2 r0 r1\n");
                    // Jump to the return address
                    sprintf(assembly[currentAssemblyLine + 8], "JMP r3 r4\n");
                    currentAssemblyLine += 9;
                }
                else if (!strcmp(token, "call"))
                {
                    token = strtok_r(NULL, " ;", &savePtr);
                    Symbol* sym = lookupSymbol(token, symbolMap);
                    if (sym != NULL)
                    {
                        uint32_t returnAssemblyLine = currentAssemblyLine + 12;
                        // Load current callstack depth
                        sprintf(assembly[currentAssemblyLine], "LRC r0 #%d\n", CALLSTACK_SEGMENT);
                        sprintf(assembly[currentAssemblyLine + 1], "LRC r1 #%d\n", CALLSTACK_DEPTH_OFFSET);
                        sprintf(assembly[currentAssemblyLine + 2], "LDR r2 r0 r1\n");
                        // Increment the callstack depth by 1 for the return segment
                        sprintf(assembly[currentAssemblyLine + 3], "ADDC r2 #1\n");
                        // Store the return segment in the callstack
                        sprintf(assembly[currentAssemblyLine + 4], "LRC r3 #%d\n", returnAssemblyLine / SEGMENT_SIZE);
                        sprintf(assembly[currentAssemblyLine + 5], "LDR r3 r0 r2\n");
                        // Increment the callstack depth by 1 for the return offset
                        sprintf(assembly[currentAssemblyLine + 6], "ADDC r2 #1\n");
                        // Store the return offset in the callstack
                        sprintf(assembly[currentAssemblyLine + 7], "LRC r3 #%d\n", returnAssemblyLine % SEGMENT_SIZE);
                        sprintf(assembly[currentAssemblyLine + 8], "LDR r3 r0 r2\n");
                        // Store the callstack depth at 63.0
                        sprintf(assembly[currentAssemblyLine + 9], "STR r2 r0 r1\n");
                        // Load the function address and jump
                        sprintf(assembly[currentAssemblyLine + 10], "LRL r0 r1 @%s\n", sym->identifier);
                        sprintf(assembly[currentAssemblyLine + 11], "JMP r0 r1\n");
                        currentAssemblyLine += 12;
                    }
                    else
                    {
                        printf("Undeclared function %s on line %d\n", token, lineCount);
                        exit(1);
                    }
                }
                else
                {
                    Symbol* sym = lookupSymbol(token, symbolMap);
                    if (sym != NULL)
                    {
                        token = strtok_r(NULL, " ", &savePtr);
                        if (!strcmp(token, "="))
                        {
                            token = strtok_r(NULL, ";", &savePtr);
                            decomposeExpression(&token, &assembly, &currentAssemblyLine, symbolMap);

                            // Store the result (r0) into the right hand side identifier
                            sprintf(assembly[currentAssemblyLine], "LRC r1 #%d\n", sym->segment);
                            sprintf(assembly[currentAssemblyLine + 1], "LRC r2 #%d\n", sym->offset);
                            sprintf(assembly[currentAssemblyLine + 2], "STR r0 r1 r2\n");
                            currentAssemblyLine += 3;
                        }
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
    dumpSymbolMap(symbolMap);
    char* noExtension = strsep(&filename, ".");
    FILE* asmfile = fopen(strcat(noExtension, ".asm"), "wb");
    for (i = 0; i < currentAssemblyLine; i++)
    {
        fwrite(assembly[i], sizeof(char), strlen(assembly[i]), asmfile);
    }
    fclose(asmfile);
    return 0;
}
