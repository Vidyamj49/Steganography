#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;
    int extn_size;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_f(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/*Encode secret file extn size*/
Status decode_extn_size(DecodeInfo *decInfo);
/* Encode secret file extenstion */
Status decode_secret_file_extn( DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real encoding */
Status decode_data_from_image(char *data, int size, int k, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status decode_byte_to_lsb(char data, char *image_buffer);

/*Decode size to lsb*/
Status decode_size_to_lsb(int size, char *image_buffer);

#endif
