#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

int main (int argc, char** argv)
{
    FILE* file = fopen("testsprite_indexed4.png", "rb");
    struct stat st;
    stat("testsprite_indexed4.png", &st);
    size_t fileSize = st.st_size;
    printf("Size: %d\n", st.st_size);
    uint8_t* image = malloc(sizeof(uint8_t) * fileSize);
    uint16_t bytesRead = fread(image, sizeof(uint8_t), fileSize, file);
    if (bytesRead != st.st_size)
    {
        printf("The file is %d bytes but only %d were read\n", st.st_size, bytesRead);
        exit(1);
    }
    if (image[1] == 'P' && image[2] == 'N' && image[3] == 'G')
        printf("yep, its a png\n");

    size_t offset = 8; // Current position looking through the image
    size_t chunkLength = 0;
    char chunkTypeStr[5];
    chunkTypeStr[4] = '\0';
    uint8_t* IDAT = NULL;
    size_t IDAT_length = 0;
    uint8_t* pHYs = NULL;
    size_t pHYs_length = 0;
    uint8_t* PLTE = NULL;
    size_t PLTE_length = 0;

    while (offset < fileSize)
    {
        // Length is big endian
        printf("Length in hex (MSByte to LSByte) is %X, %X, %X, %X\n", *(image + offset), *(image + offset + 1), *(image + offset + 2), *(image + offset + 3));
        chunkLength = 0;
        chunkLength += (*(image + offset) << 24) & 0xFF000000;
        chunkLength += (*(image + offset + 1) << 16) & 0x00FF0000;
        chunkLength += (*(image + offset + 2) << 8) & 0x0000FF00;
        chunkLength += (*(image + offset + 3)) & 0x000000FF;
        printf("Chunk Length: %u\n", chunkLength);
        offset += 4; // Length
        chunkTypeStr[0] = (*(image + offset));
        chunkTypeStr[1] = (*(image + offset + 1));
        chunkTypeStr[2] = (*(image + offset + 2));
        chunkTypeStr[3] = (*(image + offset + 3));
        printf("Chunk type: %s\n", chunkTypeStr);
        offset += 4; // Type
        if (!strcmp(chunkTypeStr, "IDAT"))
        {
            IDAT = malloc(chunkLength);
            IDAT_length = chunkLength;
            memcpy(IDAT, image + offset, chunkLength);
        }
        else if (!strcmp(chunkTypeStr, "pHYs"))
        {
            pHYs = malloc(chunkLength);
            pHYs_length = chunkLength;
            memcpy(pHYs, image + offset, chunkLength);
        }
        else if (!strcmp(chunkTypeStr, "PLTE"))
        {
            PLTE = malloc(chunkLength);
            PLTE_length = chunkLength;
            memcpy(PLTE, image + offset, chunkLength);
        }

        offset += chunkLength;
        offset += 4; // CRC
    }

    printf("IDAT:\n");
    int i;
    for (i = 0; i < IDAT_length; i++)
    {
        printf("%X", *(IDAT + i));
    }
    printf("\n");
    printf("pHYs:\n");
    for (i = 0; i < pHYs_length; i++)
    {
        printf("%X", *(pHYs + i));
    }
    printf("\n");
    printf("PLTE:\n");
    for (i = 0; i < PLTE_length; i++)
    {
        printf("%X", *(PLTE + i));
    }
    printf("\n");

    free(IDAT);
    IDAT = NULL;
    free(pHYs);
    pHYs = NULL;
    free(PLTE);
    PLTE = NULL;

    fclose(file);
    return 0;
}
