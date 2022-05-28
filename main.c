#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

int point_in_circle(float x, float y)
{
    float d = sqrt(x * x + y * y);
    return (d <= 1.0) ? 1 : 0;
}

float rand_float(float min_value, float max_value)
{
    float norm_rand_float = rand() / (float)RAND_MAX;                         // 0.0 and 1.0
    float rand_float = norm_rand_float * (max_value - min_value) + min_value; // -1.0 and 1.0
    return rand_float;
}

void *routine(void *param)
{
    int i;
    int *n_points_p = (int *)param;
    int *n_points_in_circle = malloc(sizeof(int));

    for (i = 0; i < *n_points_p; i++)
    {
        float x = rand_float(-1, 1), y = rand_float(-1, 1);
        int is_point_in_circle = point_in_circle(x, y);

        if (is_point_in_circle)
        {
            *n_points_in_circle = *n_points_in_circle + 1;
        }
    }

    return (void *)n_points_in_circle;
}

float monte_carlo_with_threads(int n_points, int n_threads)
{
    int i;
    int n_total_points_in_circle = 0;
    float pi;

    pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
    int **result = malloc(n_threads * sizeof(int *));

    int n_points_per_thread = ceil(n_points / n_threads);

    for (i = 0; i < n_threads; i++)
    {
        pthread_create(threads + i, NULL, &routine, (void *)&n_points_per_thread);
    }

    for (i = 0; i < n_threads; i++)
    {
        pthread_join(threads[i], (void **)result + i);
    }

    for (i = 0; i < n_threads; i++)
    {
        n_total_points_in_circle += *result[i];
    }

    free(threads);
    for (i = 0; i < n_threads; i++)
    {
        free(result[i]);
    }
    free(result);

    pi = (n_total_points_in_circle / (float)n_points) * 4;
    return pi;
}

// float monte_carlo(int n_points) {
//     int i;
//     int n_points_in_circle = 0;
//     float pi;

//     for (i = 0; i < n_points; i++) {
//         float x = rand_float(-1, 1), y = rand_float(-1, 1);
//         int is_point_in_circle = point_in_circle(x, y);

//         if (is_point_in_circle) {
//             n_points_in_circle++;
//         }
//     }

//     pi = (n_points_in_circle / (float) n_points) * 4;
//     return pi;
// }

int main()
{
    srand(time(NULL));

    int n_points = 1000000, n_threads = 2;
    float pi = monte_carlo_with_threads(n_points, n_threads);
    printf("The value of pi is %f\n", pi);
    return 0;
}