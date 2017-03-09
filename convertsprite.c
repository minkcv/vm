#include "constants.h"
#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int getColorIndex(int rgba[4], int colors[4][3])
{
    int i;
    for (i = 0; i < 4; i++)
    {
        // Screw &&
        if (rgba[0] == colors[i][0])
        if (rgba[1] == colors[i][1])
        if (rgba[2] == colors[i][2])
            return i;
    }
    return -1;
}

int main (int argc, char** argv)
{
    /*
    fprintf(stderr, "   Compiled with libpng %s; using libpng %s.\n",
      PNG_LIBPNG_VER_STRING, png_libpng_ver);
    fprintf(stderr, "   Compiled with zlib %s; using zlib %s.\n",
      ZLIB_VERSION, zlib_version);
    */

    if (argc < 2 || !strlen(argv[1]))
    {
        printf("Usage: convertsprite sprite.png\n");
        return 1;
    }
    char* filename = argv[1];
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Couldn't open file with name %s\n", filename);
        return 1;
    }
    char* filenameNoExtension = strsep(&filename, ".");
    if (filenameNoExtension == NULL)
    {
        printf("Filename should have \".png\" as an extension\n");
        return 1;
    }

    FILE* spriteFile = fopen(strcat(filenameNoExtension, ".spr"), "wb");
    int x, y;
    int width, height;
    png_byte colorType;
    png_byte bitDepth;
    png_structp pngPtr;
    png_infop infoPtr;
    int numberOfPasses;
    png_bytep* rowPointers;
    pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    infoPtr = png_create_info_struct(pngPtr);
    setjmp(png_jmpbuf(pngPtr));
    png_init_io(pngPtr, fp);
    png_set_sig_bytes(pngPtr, 0);
    png_read_info(pngPtr, infoPtr);
    width = png_get_image_width(pngPtr, infoPtr);
    height = png_get_image_height(pngPtr, infoPtr);
    colorType = png_get_color_type(pngPtr, infoPtr);
    bitDepth = png_get_bit_depth(pngPtr, infoPtr);
    numberOfPasses = png_set_interlace_handling(pngPtr);
    png_read_update_info(pngPtr, infoPtr);
    setjmp(png_jmpbuf(pngPtr));
    rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (y = 0; y < height; y++)
        rowPointers[y] = (png_byte*)malloc(png_get_rowbytes(pngPtr, infoPtr));
    png_read_image(pngPtr, rowPointers);

    int usedColors = 0;
    int colors[4][3];
    memset(colors, -1, sizeof(int) * 4 * 3);
    int rgba[4] = {};
    uint8_t* sprite = (uint8_t*)malloc((width / 4) * height);
    size_t spriteOffset = 0;
    int currentBits = 3; // 4 pairs of bits in a byte
    int useAlpha = 0;
    for (y = 0; y < height; y++)
    {
        png_byte* row = rowPointers[y];
        for (x = 0; x < width; x++)
        {
            png_byte* pixel = &(row[x * 4]); // RGBA
            rgba[0] = pixel[0];
            rgba[1] = pixel[1];
            rgba[2] = pixel[2];
            rgba[3] = pixel[3];
            int index;
            if (rgba[3] == 0) // Transparent
            {
                index = 3;
                useAlpha = 1;
            }
            else
            {
                index = getColorIndex(rgba, colors); // 2 bit index into colors[4]
                if (index == -1)
                {
                    colors[usedColors][0] = rgba[0];
                    colors[usedColors][1] = rgba[1];
                    colors[usedColors][2] = rgba[2];
                    index = usedColors;
                    usedColors++;
                }
            }
            sprite[spriteOffset] |= index << (2 * currentBits);
            currentBits--;
            if (currentBits < 0)
            {
                currentBits = 3;
                spriteOffset++;
            }
        }
    }

    size_t written = fwrite(sprite, sizeof(uint8_t), (width  / 4) * height, spriteFile);
    printf("Wrote %d bytes to %s\n", written, strcat(filenameNoExtension, ".spr"));
    fclose(spriteFile);
    free(sprite);
    for (y = 0; y < height; y++)
        free(rowPointers[y]);
    free(rowPointers);

    fclose(fp);

    printf("This sprite's color palette is:\n");
    int i;
    for (i = 0; i < 4 - useAlpha; i++)
    {
        // Look in the gimp palette to gain insight into this.
        int fullPaletteIndex = ((colors[i][0] / 36.4285) * 32) + ((colors[i][1] / 36.4285) * 4) + (colors[i][2] / 85.0);
        printf("Color %d has palette index %d\n", i, fullPaletteIndex);
    }
    if (useAlpha)
        printf("This sprite uses color 4 as transparency\n");

    return 0;
}
