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
            char* token = strtok(lineIn, " ;\n");
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
                    token = strtok(NULL, " ;");
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
                    token = strtok(NULL, " {");
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
                    printf("going to add %s to symbol map\n", token);
                    addSymbolToMap(token, &symbolMap);
                    printf("going to add label %s to line %d of the assembly\n", token, currentAssemblyLine);
                    sprintf(assembly[currentAssemblyLine], "@%s\n", token);
                    printf("added a label to the assembly\n");
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
                    token = strtok(NULL, " ;");
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
                        sprintf(assembly[currentAssemblyLine + 4], "LRC r3 %d\n", returnAssemblyLine / SEGMENT_SIZE);
                        sprintf(assembly[currentAssemblyLine + 5], "LDR r3 r0 r2\n");
                        // Increment the callstack depth by 1 for the return offset
                        sprintf(assembly[currentAssemblyLine + 6], "ADDC r2 #1\n");
                        // Store the return offset in the callstack
                        sprintf(assembly[currentAssemblyLine + 7], "LRC r3 %d\n", returnAssemblyLine % SEGMENT_SIZE);
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
                        token = strtok(NULL, " ;");
                        if (!strcmp(token, "="))
                        {
                        }
                    }
                }

                // Consume any trailing spaces
                token = strtok(NULL, " ");
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
