__kernel void render(__global float* fb, int max_x, int max_y) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if ((i < max_x) && (j < max_y)) {
        int pixel_index = j * max_x * 3 + i * 3;
        fb[pixel_index + 0] = (float)i / max_x;
        fb[pixel_index + 1] = (float)j / max_y;
        fb[pixel_index + 2] = 0.2;
    }
}