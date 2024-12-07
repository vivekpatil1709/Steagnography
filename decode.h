#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *decode_src_image_fname;
    FILE *decode_fptr_src_image;

    /* Secret File Info */
    char decode_secret_fname[20];
    FILE *decode_fptr_secret;

} DecodeInfo;

/* Perform the encoding */
Status do_decoding(DecodeInfo *decodeInfo);

/* Get File pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo *decodeInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decodeInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decodeInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decodeInfo);

/* Encode secret file data*/
Status decode_secret_file_data(int ,DecodeInfo *decodeInfo,char *);

/* Encode function, which does the real encoding */
//Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status decode_byte_to_msb(char *);

Status decode_int_to_msb(char *);

#endif