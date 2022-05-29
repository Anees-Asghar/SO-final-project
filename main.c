#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>


// Returns 1 if point (x, y) is on or inside the circle of radius 1, otherwise returns 0
int point_in_circle(float x, float y) {
    float distance_from_origin = sqrt(x*x + y*y);
    return (distance_from_origin <= 1.0) ? 1 : 0;
}


// Returns a random float between the range min_value and max_value
float rand_float(float min_value, float max_value) {
    float norm_rand_float = rand() / (float) RAND_MAX; // random float between 0.0 and 1.0
    float rand_float = norm_rand_float * (max_value - min_value) + min_value; // random float in the requested range
    return rand_float;
}


// Thread routine to generate a specified number of points and return a pointer to the 
// total number of those points that fall on or inside the circle of radius 1
void *routine(void *param) {
    int *p_n_points = (int *)param; // pointer to the number of points to generate
    int i = 0, n_points_in_circle = 0;
    int* p_result = malloc(sizeof(int*)); // allocate space in memory for the result (n_points_in_circle)

    // generate points and increment n_points_in_circle if point falls on or inside the circle
    for (i = 0; i < *p_n_points; i++) {
        float x = rand_float(-1.0, 1.0), y = rand_float(-1.0, 1.0);
        if (point_in_circle(x, y)) {
            n_points_in_circle++;
        }
    }

    *p_result = n_points_in_circle;
    return (void*)p_result;
}


// Simulates monte carlo with n_points points across n_threads threads
// and returns an approximation of pi
float monte_carlo(int n_points, int n_threads) {
    double pi;
    int i, total_n_points_in_circle = 0;
    int n_points_per_thread  = ceil((double)n_points / n_threads); // calculate the number of random points each thread has to generate
    n_points = n_points_per_thread * n_threads;

    pthread_t *thread_ids = malloc(n_threads * sizeof(pthread_t)); // array of size n_threads to store pthread_t
    int **result = malloc(n_threads * sizeof(int *)); // array of size n_threads where each element stores the
                                                      // return value of a thread

    // create threads
    for (i = 0; i < n_threads; i++) {
        pthread_create(thread_ids+i, NULL, &routine, (void *)&n_points_per_thread);
    }

    // await threads
    for (i = 0; i < n_threads; i++) {
        pthread_join(thread_ids[i], (void **) result+i);
    }

    // calculate total number of points in circle (across all threads)
    for (i = 0; i < n_threads; i++) {
        total_n_points_in_circle += *result[i];
    }

    // free dynamically allocated memory
    free(thread_ids);
    for (i = 0; i < n_threads; i++) {
        free(result[i]); // allocated inside thread routine
    }
    free(result);

    pi = ((double)total_n_points_in_circle / n_points) * 4; // compute pi
    
    return pi;
}


int main() {
    srand(time(NULL));

    int i, j, n_points, n_threads;
    double approx_pi;
    int thread_options[] = {2, 4, 6, 8};
    int point_options[] = {20000, 100000, 1000000, 10000000};

    // simulate monte carlo and print performance metrics for each pair of options
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++) {
            n_points = point_options[i];
            n_threads = thread_options[j];

            approx_pi = monte_carlo(n_points, n_threads);
            printf("No. of points: %d\n", n_points);
            printf("No. of threads: %d\n", n_threads);
            printf("Approximation of pi: %lf\n\n", approx_pi);
        }
    }

    return 0;
}
