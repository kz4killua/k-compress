#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"


void reconstruct_image(int width, int height, RGBTRIPLE image[][width], RGBTRIPLE centroids[], uint8_t assignments[]);
int write_output(char const *path, BITMAPFILEHEADER bf, BITMAPINFOHEADER bi, int width, int height, RGBTRIPLE image[][width]);


int main(int argc, char const *argv[])
{
    // Ensure correct usage
    if (argc != 3)
    {
        printf("Error: Usage: ./decompress infile outfile\n");
        return 1;
    }
 
    char const *input_path = argv[1];
    char const *output_path = argv[2];

    // Open the file
    FILE *input = fopen(input_path, "rb");
    if (input == NULL)
    {
        printf("Error: Could not open %s.\n", input_path);
        fclose(input);
        return 2;
    }

    // Load the BITMAPFILEHEADER and BITMAPINFOHEADER
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, input);
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, input);

    // Check the number of pixels (m)
    int m;
    fread(&m, sizeof(int), 1, input);

    // Read the pixel assignments
    uint8_t assignments[m];
    fread(assignments, sizeof(uint8_t), m, input);

    // Check the number of centroids (k)
    int k;
    fread(&k, sizeof(int), 1, input);

    // Read the centroids
    RGBTRIPLE centroids[k];
    fread(centroids, sizeof(RGBTRIPLE), k, input);

    // Close the input file.
    fclose(input);
    
    // Reconstruct the image
    int height = abs(bi.biHeight);
    int width = bi.biWidth;
    RGBTRIPLE image[height][width];
    reconstruct_image(width, height, image, centroids, assignments);

    // Output the image
    int code = write_output(output_path, bf, bi, width, height, image);
    if (code)
    {
        printf("Error: Failed to write output.\n");
        return 3;
    }

    return 0;
}


/* Reconstructs an image. */
void reconstruct_image(int width, int height, RGBTRIPLE image[][width], RGBTRIPLE centroids[], uint8_t assignments[])
{
    // Fill in the pixels
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = centroids[assignments[i * width + j]];
        }
    }
}


/* Saves an image to a BMP file. */
int write_output(char const *path, BITMAPFILEHEADER bf, BITMAPINFOHEADER bi, int width, int height, RGBTRIPLE image[][width])
{
    // Open the file
    FILE *output = fopen(path, "wb");
    if (output == NULL)
    {
        return 1;
    }

    // Write the BITMAPFILEHEADER and BITMAPINFOHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, output);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, output);

    // Write the image pixels
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;
    for (int i = 0; i < height; i++)
    {
        // Write each row of pixels
        fwrite(image[i], sizeof(RGBTRIPLE), width, output);
        // Write padding
        for (int j = 0; j < padding; j++)
        {
            fputc(0x00, output);
        }
    }

    fclose(output);

    return 0;
}