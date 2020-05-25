#include <time.h>
#include <png.h>
#include <stdlib.h>

#include "img.h"

int readpng(const char* file_name, struct img* img_ptr) {
    FILE *fp = fopen(file_name, "rb");
    if (!fp) return ERROR_FILE_OPEN_ERR;

    // Read 8 bytes of header and check if this is _probably_ a png
    int number = 8;
    void* header = malloc(number);
    fread(header, 1, number, fp);
    int is_png = !png_sig_cmp(header, 0, number);
    if (!is_png) return ERROR_NOT_PNG;

    // Setup and handle errors for the read pointer
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
            NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return ERROR_PNG_INIT_ERR;
    }

    // Setup and handle errors for the info pointer
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL,
                (png_infopp) NULL);
        fclose(fp);
        return ERROR_PNG_INIT_ERR;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return ERROR_PNG_INIT_ERR;
    }

    // libpng error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return ERROR_PNG_INIT_ERR;
    }

    png_init_io(png_ptr, fp);

    png_set_sig_bytes(png_ptr, number);

    png_read_info(png_ptr, info_ptr);

    // Allocate pixel array
    img_ptr->width = png_get_image_width(png_ptr, info_ptr);
    img_ptr->height = png_get_image_height(png_ptr, info_ptr);
    img_ptr->rows = calloc(sizeof(struct pixel*), img_ptr->height);
    for (int row = 0; row < img_ptr->height; row++) {
        img_ptr->rows[row] = calloc(sizeof(struct pixel), img_ptr->width);
    }

    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    // int channels = png_get_channels(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY) {
       if (bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
       }
       png_set_gray_to_rgb(png_ptr);
    }

    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }

    png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    png_read_image(png_ptr,(png_bytepp) img_ptr->rows);

    png_read_end(png_ptr, end_info);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    fclose(fp);

    return SUCCESS;
}

int writepng(const char* file_name, struct img* img_ptr) {
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
