#ifndef IMG_H
#define IMG_H

#include <png.h>

// 8 bits per color, no transparancy
struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

// pixels are stored in an array of rows
struct img {
    int width;
    int height;
    struct pixel** rows;
};

enum status {
    SUCCESS = 0,

    ERROR,
    ERROR_FILE_OPEN_ERR,
    ERROR_PNG_INIT_ERR,
};


// Writes the image defined in img_ptr to the file referred to by file_name.
// Returns an integer representing the result status of the writing.
int writepng(const char* file_name, struct img* img_ptr);

#endif
