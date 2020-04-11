#include <time.h>
#include <png.h>

#include "img.h"

int writepng(char* file_name, struct img* img_ptr) {
    FILE *fp = fopen(file_name, "wb");
    if (!fp) return ERROR_FILE_OPEN_ERR;

    // Setup and handle errors for the write pointer
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
            NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return ERROR_PNG_INIT_ERR;
    }

    // Setup and handle errors for the info pointer
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
        return ERROR_PNG_INIT_ERR;
    }

    // libpng error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return ERROR_PNG_INIT_ERR;
    }

    png_init_io(png_ptr, fp);


    // TODO consider supporting variable color setups
    png_set_IHDR(png_ptr, info_ptr, img_ptr->width, img_ptr->height, 8,
            PNG_COLOR_TYPE_RGB, PNG_INTERLACE_ADAM7,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Set modification time to the current system time
    const time_t time_0 = time(NULL);
    struct tm* ltime = localtime(&time_0);
    png_time mod_time;
    png_convert_from_struct_tm(&mod_time, ltime);
    png_set_tIME(png_ptr, info_ptr, &mod_time);

    // Write the image data
    png_set_rows(png_ptr, info_ptr, (png_bytepp) img_ptr->rows);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return SUCCESS;
}
