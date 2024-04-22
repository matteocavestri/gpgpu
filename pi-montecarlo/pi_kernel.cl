__kernel void compute_pi(__global int *count, const int num_samples, __global unsigned int *seeds) {
    int id = get_global_id(0);
    int local_count = 0;
    float x, y;
    
    // Generatore di numeri casuali semplice
    unsigned int seed = seeds[id];  // seed diverso per ogni work-item

    for (int i = 0; i < num_samples; ++i) {
        x = (float)rand_r(&seed) / RAND_MAX;
        y = (float)rand_r(&seed) / RAND_MAX;
        if (x * x + y * y <= 1.0f) {
            local_count++;
        }
    }

    count[id] = local_count;
}

