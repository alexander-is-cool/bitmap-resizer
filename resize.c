// Copies a BMP file
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bmp.h"

// Checks if num is a valid float
int isfloat(char *num);

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 4 || !isfloat(argv[1]))
    {
        fprintf(stderr, "Usage: resize.exe scale infile outfile\n");
        return 1;
    }

    // Remember filenames
    double f = atof(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    if (f < 0 || 100 < f)
    {
        fprintf(stderr, "Usage: resize.exe scale infile outfile\n");
        return 1;
    }

    //printf("%f\n", f); return 0;

    // Open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // Open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // Read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // Creates new version of BITMAPFILEHEADER and BITMAPINFOHEADER
    BITMAPFILEHEADER bf_new = bf;
    BITMAPINFOHEADER bi_new = bi;

    // Modifys new versions dimensions
    bi_new.biHeight = round(bi.biHeight * f);
    bi_new.biWidth = round(bi.biWidth * f);

    // Determine padding for old and new image
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding_new = (4 - (bi_new.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Modifys new versions size
    bi_new.biSizeImage = ((sizeof(RGBTRIPLE) * bi_new.biWidth) + padding_new) * abs(bi_new.biHeight);
    bf_new.bfSize = bi_new.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Write outfile's modified BITMAPFILEHEADER
    fwrite(&bf_new, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's modified BITMAPINFOHEADER
    fwrite(&bi_new, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Array that contains the pixels of infile
    RGBTRIPLE pixels[abs(bi.biHeight) * bi.biWidth];

    // Iterate over infile's heigth
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // Iterate over infile's width
        for (int j = 0; j < bi.biWidth; j++)
        {
            // read RGB triple from infile and puts it in pixel array
            fread(&pixels[(i * bi.biWidth + j)], sizeof(RGBTRIPLE), 1, inptr);
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }

    float x, y;
    int x2, y2;

    // Iterate over outfiles's heigth
    for (int i = 0, biHeight = abs(bi_new.biHeight); i < biHeight; i++)
    {
        y = i / f;
        y2 = y;

        // Iterate over outfiles's width
        for (int j = 0; j < bi_new.biWidth; j++)
        {
            x = j / f;
            x2 = x;

            // Writes nearest pixel to outfile
            fwrite(&pixels[(y2 * bi.biWidth) + x2], sizeof(RGBTRIPLE), 1, outptr);
        }

        // Adds pading to the width, if any
        for (int k = 0; k < padding_new; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

// Checks if num is a valid float
int isfloat(char *num)
{
    // Iterates over num or a dot
    for (int i = 0, len = strlen(num); i < len; i++)
    {
        // Checks num[i] is a digit
        if (!isdigit(num[i]) && num[i] != '.')
        {
            // Returns false
            return 0;
        }
    }

    // Returns true
    return 1;
}