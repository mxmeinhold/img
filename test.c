#include <stdlib.h>

#include "img.h"

// Test writing by generating a color gradient.
void print_colors(void) {
    struct img out_img;
    out_img.width = 255;
    out_img.height = 255;

    out_img.rows = calloc(sizeof(struct pixel*), out_img.height);
    for (int row = 0; row < out_img.height; row++) {
        out_img.rows[row] = calloc(sizeof(struct pixel), out_img.width);
        for (int col = 0; col < out_img.width; col++) {
            struct pixel p;
            p.red = row % 255;
            p.blue = col % 255;
            p.green = 255 - ((p.red > p.blue)? p.red:p.blue);
            out_img.rows[row][col] = p;
        }
    }

    writepng("colors.png", &out_img);
#ifdef INCL_QOI
    writeqoi("colors.qoi", &out_img);
#endif

    for (int row = 0; row < out_img.height; row++) {
        free(out_img.rows[row]);
    }
    free(out_img.rows);
}

// Test reading by reading the written image and writing it out again
// TODO this isn't a real unit test
void read_write(void) {
    struct img read_img;

    readpng("colors.png", &read_img);
#ifdef INCL_QOI
    writeqoi("read_write.qoi", &read_img);

    readqoi("read_write.qoi", &read_img);
#endif
    writepng("read_write.png", &read_img);

    for (int row = 0; row < read_img.height; row++) {
        free(read_img.rows[row]);
    }
    free(read_img.rows);
}

int main(int argc, char** argv) {
    print_colors();
    read_write();
    return 0;
}
