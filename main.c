#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

int point_in_circle(float x, float y) {
    float d = sqrt(x*x + y*y);
    return (d <= 1.0) ? 1 : 0;
}


float rand_float(float min_value, float max_value) {
    float norm_rand_float = rand() / (float) RAND_MAX; // 0.0 and 1.0
    float rand_float = norm_rand_float * (max_value - min_value) + min_value; // -1.0 and 1.0
    return rand_float;
}


void *routine(void *param) {
    int i;
    int *n_points_p = (int *) param;
    int* n_points_in_circle = malloc(sizeof(int*));

    for (i = 0; i < *n_points_p; i++) {
        float x = rand_float(-1, 1), y = rand_float(-1, 1);
        int is_point_in_circle = point_in_circle(x, y);

        if (is_point_in_circle) {
            printf("num_points_b4: %ld\n",*n_points_in_circle);
            *n_points_in_circle=*n_points_in_circle+1;
            printf("num_points_after: %ld\n",*n_points_in_circle);
        }
    }

    return (void*) n_points_in_circle;
}

float monte_carlo(int n_points, int n_threads) {
    int i = 0, n_total_points_in_circle = 0;
    double pi;
    
    pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
    int **result = malloc(n_threads * sizeof(int *));

    int n_points_per_thread  = ceil((double) n_points / n_threads); //the result of operation has to be a double

    for (i = 0; i < n_threads; i++) {
        pthread_create(threads+i, NULL, &routine, (void *) &n_points_per_thread);
    }

    for (i = 0; i < n_threads; i++) {
        pthread_join(threads[i], (void **) result+i);
    }
 

    for (i = 0; i < n_threads; i++) {
        printf("result: %d, i: %d\n", *result[i], i);
        n_total_points_in_circle += *result[i];
    }
    printf("num points in circle: %d, num point thread: %d, threads: %d\n",n_total_points_in_circle, n_points_per_thread, n_threads);

    free(threads);
    for (i = 0; i < n_threads; i++) {
        free(result[i]);
    }
    free(result);
            
    pi = (n_total_points_in_circle / (double)(n_points_per_thread * n_threads)) * 4;
    return pi;
}

int main() {
    srand(time(NULL));

    int i, j;
    float error_rate;

    int n_threads_settings[] = {2, 4, 6, 8};
    int n_points_settings[] = {20, 100000, 1000000, 10000000};

    for (i = 0; i < 1; i++){ //4
        int n_points = n_points_settings[i];
        for (j = 0; j < 4; j++) {
            int n_threads = n_threads_settings[j];

            double approx_pi = monte_carlo(n_points, n_threads);

            // error_rate = (M_PI - approx_pi) / M_PI * 100;

            // printf("--------------------------------------\n");
            // printf("Settings:\tn_threads = %d\tn_points = %d\n", n_threads, n_points);
            // printf("The approximation of pi using these settings is %lf.\n", approx_pi);
            // printf("The error rate for this approximation of pi is %f%%\n", error_rate);

            printf("For %d threads and %d points the approximation of pi is %lf\n\n", n_threads, n_points, approx_pi);
        }
    }

    return 0;
}
