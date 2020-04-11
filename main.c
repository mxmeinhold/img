#include "img.h"
#include <stdlib.h>

// Small test writing program
// Generates a nice little color test
int main() {
    struct img out_img;
    out_img.width = 255;
    out_img.height = 255;

    out_img.rows = calloc(sizeof(struct pixel*), out_img.height);
    for (int row = 0; row < out_img.height; row++) {
        out_img.rows[row] = calloc(sizeof(struct pixel), out_img.width);
        for (int col = 0; col < out_img.width; col++) {
            struct pixel p;
            p.red = row;
            p.blue = col;
            p.green = 255 - ((row > col)? row:col);
            out_img.rows[row][col] = p;
        }
    }

    writepng("test.png", &out_img);

    for (int row = 0; row < out_img.height; row++) {
        free(out_img.rows[row]);
    }
    free(out_img.rows);
}
