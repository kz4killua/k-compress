#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>


const int K = 64;
const int ITERATIONS = 10;


void initialize_centroids(RGBTRIPLE centroids[], int k, RGBTRIPLE x[], int m);
int closest_centroid(RGBTRIPLE example, RGBTRIPLE centroids[], int k);
void recompute_centroids(RGBTRIPLE centroids[], uint8_t assignments[], RGBTRIPLE x[], int k, int m);
float distance(RGBTRIPLE a, RGBTRIPLE b, int size);


void kmeans(RGBTRIPLE x[], int m, int k, int iterations, RGBTRIPLE centroids[], uint8_t assignments[])
{

    // Initialize the cluster centroids
    initialize_centroids(centroids, k, x, m);
    
    // Run K-means clustering
    for (int i = 0; i < iterations; i++)
    {
        printf("Iteration: %i/%i\n", i + 1, ITERATIONS);

        // Assign each example to its closest cluster centroid
        for (int j = 0; j < m; j++)
        {
            RGBTRIPLE example = x[j];
            int c = closest_centroid(example, centroids, k);
            assignments[j] = c;
        }

        // Recompute cluster centroids
        recompute_centroids(centroids, assignments, x, k, m);
    }
}


/* Randomly initializes K cluster centroids */
void initialize_centroids(RGBTRIPLE centroids[], int k, RGBTRIPLE x[], int m)
{
    for (int i = 0; i < k; i++)
    {
        // Pick a random training example
        int a = rand() % m;

        // Use to initialize a cluster centroid
        centroids[i] = x[a];
    }
}


/* Returns the *index* of the closest cluster centroid */
int closest_centroid(RGBTRIPLE example, RGBTRIPLE centroids[], int k)
{
    float min_d = FLT_MAX;
    int min_c = 0;

    for (int i = 0; i < k; i++)
    {
        RGBTRIPLE c = centroids[i];
        // Check the distance between each centroid and the example
        float d = distance(c, example, 3);
        // Update the closest centroid
        if (d < min_d)
        {
            min_d = d;
            min_c = i;
        }
    }

    return min_c;
}


/* Recomputes cluster centroids, setting each to the mean of its examples. */
void recompute_centroids(RGBTRIPLE centroids[], uint8_t assignments[], RGBTRIPLE x[], int k, int m)
{

    int sums[k][3];
    memset(sums, 0, k * 3 * sizeof(int));
    int counts[k];
    memset(counts, 0, k * sizeof(int));

    // Go over each example in the dataset
    for (int i = 0; i < m; i++)
    {
        // Check which cluster assignement it is
        int c = assignments[i];
        // Update `sums` with the B, G, and R values
        sums[c][0] += x[i].rgbtBlue;
        sums[c][1] += x[i].rgbtGreen;
        sums[c][2] += x[i].rgbtRed;
        // Update `counts`
        counts[c] += 1;
    }

    // Update each centroid with the average of its assigned examples
    for (int i = 0; i < k; i++)
    {
        centroids[i].rgbtBlue  = counts[i] ? (int) (sums[i][0] / counts[i]) : 0;
        centroids[i].rgbtGreen = counts[i] ? (int) (sums[i][1] / counts[i]) : 0;
        centroids[i].rgbtRed   = counts[i] ? (int) (sums[i][2] / counts[i]) : 0;
    }
}


/* Returns the Euclidian distance between two RGBTRIPLEs */
float distance(RGBTRIPLE a, RGBTRIPLE b, int size)
{
    int d = 0;

    d += pow(a.rgbtBlue - b.rgbtBlue, 2);
    d += pow(a.rgbtGreen - b.rgbtGreen, 2);
    d += pow(a.rgbtRed - b.rgbtRed, 2);

    return (float) sqrt(d);
}
