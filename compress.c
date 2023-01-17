#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include "kmeans.c"


RGBTRIPLE *flatten(int height, int width, RGBTRIPLE image[][width]);
int write_output(char const *path, RGBTRIPLE centroids[], uint8_t assignments[], int m, BITMAPFILEHEADER bf, BITMAPINFOHEADER bi);


int main(int argc, char const *argv[])
{
    // Ensure correct usage
    if (argc != 3)
    {
        printf("Error: Usage: ./compress infile outfile\n");
        return 1;
    }
    char const *input_path = argv[1];
    char const *output_path = argv[2];

    // Open the input file
    FILE *input = fopen(input_path, "rb");
    if (input == NULL)
    {
        printf("Error: Could not open %s.\n", input_path);
        return 2;
    }

    // Read BITMAPFILEHEADER and BITMAPINFOHEADER
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, input);
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, input);

    // Ensure the input is a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(input);
        printf("Error: Unsupported image format.\n");
        return 2;
    }

    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // Allocate memory for the image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        fclose(input);
        printf("Error: Not enough memory to store image.\n");
        return 2;
    }

    // Read the file into the image array
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;
    for (int i = 0; i < height; i++)
    {
        fread(image[i], sizeof(RGBTRIPLE), width, input);
        fseek(input, padding, SEEK_CUR);
    }

    // Close the input file
    fclose(input);

    // Flatten the 2D image
    RGBTRIPLE *flattened_image = flatten(height, width, image);
    if (flattened_image == NULL)
    {
        printf("Failed to flatten image.\n");
        return 3;
    }
    
    // Perform k-means clustering
    int m = height * width;
    RGBTRIPLE centroids[K];
    uint8_t assignments[m];
    kmeans(flattened_image, m, K, ITERATIONS, centroids, assignments);

    // Write the output to a file
    int code = write_output(output_path, centroids, assignments, m, bf, bi);
    if (code)
    {
        printf("Error: Failed to write output.\n");
        return 4;
    }

    // Free all allocated memory
    free(flattened_image);
    free(image);
    
    return 0;
}


/* Flattens a 2D array of RGBTRIPLEs */
RGBTRIPLE *flatten(int height, int width, RGBTRIPLE image[][width])
{
    // Allocate space for a 1D array
    RGBTRIPLE *flattened_image = malloc(height * width * sizeof(RGBTRIPLE));
    if (flattened_image == NULL)
    {
        return NULL;
    }

    // Copy over the RGBTRIPLEs
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            flattened_image[(i * width) + j] = image[i][j];
        }
    }

    return flattened_image;
}


/* Write compressed image data to a file. */
int write_output(char const *path, RGBTRIPLE centroids[], uint8_t assignments[], int m, BITMAPFILEHEADER bf, BITMAPINFOHEADER bi)
{
    // Open the output file
    FILE *output = fopen(path, "wb");
    if (output == NULL)
    {
        fclose(output);
        return 1;
    }

    // Write the BITMAPFILEHEADER and BITMAPINFOHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, output);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, output);

    // Write number of pixels (m)
    fwrite(&m, sizeof(int), 1, output);

    // Write pixel assignments
    fwrite(assignments, sizeof(uint8_t), m, output);

    // Write the number of centroids (k)
    fwrite(&K, sizeof(int), 1, output);

    // Write the centroids
    fwrite(centroids, sizeof(RGBTRIPLE), K, output);

    fclose(output);

    return 0;
}