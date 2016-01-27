//Siyuan Zhou
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t Byte;
typedef uint32_t DWORD;
typedef int32_t Long;
typedef uint16_t Word;

typedef struct {
    Word Type;
    DWORD Size;
    Word Reserve1;
    Word Reserve2;
    DWORD Offset;
} __attribute__((__packed__))
BMPfileheader;

typedef struct {
    DWORD headersize;
    Long width;
    Long height;
    Word colorplanes;
    Word bitsperpixel;
    DWORD compression;
    DWORD imagesize;
    Long horizontal;
    Long vertical;
    DWORD colorpalette;
    DWORD importantcolor;
} __attribute__((__packed__))
BMPinfoheader;

typedef struct {
    Byte Blue;
    Byte Green;
    Byte Red;
} __attribute__((__packed__))
RGB;

void smallbmp() {
    FILE* input = fopen("test.bmp", "rb");
    FILE *output_small = fopen("small.bmp", "wb");

    char firstField[2], secondField[12], thirdField[8], fourthField[16];
    int filesize, small_width, small_height, imagesize;

    fread(firstField, sizeof (char), 2, input);
    fread(&filesize, sizeof (int), 1, input);
    fread(secondField, sizeof (char), 12, input);
    fread(&small_width, sizeof (int), 1, input);
    fread(&small_height, sizeof (int), 1, input);
    fread(thirdField, sizeof (char), 8, input);
    fread(&imagesize, sizeof (int), 1, input);
    fread(fourthField, sizeof (char), 16, input);

    char colorData[small_height][small_width * 3];
    char smallColorData[small_height / 2][(small_width * 3) / 2 ];

    fread(colorData, sizeof (char), small_height * small_width * 3, input);

    int row, column;
    char temp;
    for (column = 0; column < small_width * 3; column++) {
        for (row = 0; row < small_height; row++) {
            temp = colorData[row][column];
            smallColorData[row / 2][column / 2] = temp;
        }
    }

    int smallFileSize = (imagesize / 4) + 54;
    int smallWidth = small_width / 2;
    int smallHeight = small_height / 2;
    int smallImageSize = smallWidth * smallHeight;

    fwrite(firstField, sizeof (char), 2, output_small);
    fwrite(&smallFileSize, sizeof (int), 1, output_small);
    fwrite(secondField, sizeof (char), 12, output_small);
    fwrite(&smallWidth, sizeof (int), 1, output_small);
    fwrite(&smallHeight, sizeof (int), 1, output_small);
    fwrite(thirdField, sizeof (char), 8, output_small);
    fwrite(&smallImageSize, sizeof (int), 1, output_small);
    fwrite(fourthField, sizeof (char), 16, output_small);
    fwrite(smallColorData, sizeof (char), (small_height * small_width * 3), output_small);

    fclose(output_small);
}

int main(void) {

    FILE* input = fopen("test.bmp", "rb");
    FILE *output_big = fopen("big.bmp", "wb");
    // FILE *output_small = fopen("small.bmp", "wb");

    BMPfileheader fileheader;
    fread(&fileheader, sizeof (BMPfileheader), 1, input);
    BMPinfoheader infoheader;
    fread(&infoheader, sizeof (BMPinfoheader), 1, input);

    fileheader.Size = sizeof (BMPfileheader) + sizeof (BMPinfoheader) +
            (infoheader.imagesize * 2);

    int big_height = infoheader.height;
    int big_width = infoheader.width;

    infoheader.height *= 2;
    infoheader.width *= 2;
    infoheader.imagesize *= 2;

    fwrite(&fileheader, sizeof (BMPfileheader), 1, output_big);
    fwrite(&infoheader, sizeof (BMPinfoheader), 1, output_big);

    int big_pad = (4 - (big_width * sizeof (RGB)) % 4) % 4;
    int big_outpad = (4 - (infoheader.width * sizeof (RGB)) % 4) % 4;

    for (int i = 0, absHeight = abs(big_height); i < absHeight; i++) {
        fpos_t big_position;
        fgetpos(input, &big_position);
        for (int m = 0; m < 2; m++) {
            if (m < 2)
                fsetpos(input, &big_position);
            int j = 0;
            while (j < big_width) {
                RGB big_temp;
                fread(&big_temp, sizeof (RGB), 1, input);
                int n = 0;
                while (n < 2) {
                    fwrite(&big_temp, sizeof (RGB), 1, output_big);
                    n++;
                }
                j++;
            }
            fseek(input, big_pad, SEEK_CUR);
            int k = 0;
            while (k < big_outpad) {
                fputc(0x00, output_big);
                k++;
            }
        }
    }

    smallbmp();

    fclose(input);
    fclose(output_big);

    return 0;
}
