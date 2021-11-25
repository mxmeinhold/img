#ifndef IMG_H
#define IMG_H

// 8 bits per color, no transparancy
struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

// pixels are stored in an array of row arrays
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
    ERROR_NOT_PNG
};


// Writes the image defined in img_ptr to the file referred to by file_name.
// Returns an integer representing the result status of the writing.
int writepng(const char* file_name, struct img* img_ptr);


// Reads an image from the specified file
// Returns an integer representing the result status of the read.
int readpng(const char* file_name, struct img* img_ptr);

#endif // IMG_H
