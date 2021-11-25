#include <string.h> // memcpy()

#define QOI_IMPLEMENTATION
#include "qoi/qoi.h"

#include "img.h"

// Writes the image defined in img_ptr to the file referred to by file_name
// using the Quite OK Image format (see https://github.com/phoboslab/qoi)
// Returns an integer representing the result status of the writing.
int writeqoi(const char* file_name, struct img* img_ptr){
    unsigned char* data = malloc(sizeof(struct pixel) * img_ptr->width * img_ptr->height);
    for (int row = 0; row < img_ptr->height; row++) {
        for (int col = 0; col < img_ptr->width; col++) {
           memcpy(data + (row * img_ptr->width + col) * sizeof(struct pixel), img_ptr->rows[row] + col, sizeof(struct pixel));
        }
    }
    int num_written = qoi_write(file_name, (void*)data, img_ptr->width, img_ptr->height, 3);
    free(data);
    if (num_written != 0) {
        return SUCCESS;
    }
    return ERROR;
}


// Reads an image from the specified file using the Quite OK Image format
// (see https://github.com/phoboslab/qoi)
// Returns an integer representing the result status of the read.
int readqoi(const char* file_name, struct img* img_ptr){
    unsigned char* data = qoi_read(file_name, &img_ptr->width, &img_ptr->height, 3);
    if (data == NULL) {
        return ERROR;
    }

    for (int row = 0; row < img_ptr->height; row++) {
        for (int col = 0; col < img_ptr->width; col++) {
           memcpy(img_ptr->rows[row] + col, data + (row * img_ptr->width + col) * sizeof(struct pixel), sizeof(struct pixel));
        }
    }

    free(data);
    return SUCCESS;
}
