


float lerpRange(float x, float x0, float x1, float y0, float y1) {
    return ((y1 - y0) * x + x1 * y0 - x0 * y1) / (x1 - x0);
}